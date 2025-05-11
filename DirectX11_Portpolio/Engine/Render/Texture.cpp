#include "HeaderCollection.h"
#include "Texture.h"
#include <directxtk/DDSTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

using namespace DirectX;

Texture::Texture(wstring InFileName, TexMetadata& InLoadInfo,
						bool bUseSRGB, bool bIsCubeMap, bool bDefaultPath)
{
	FileName = InFileName;

	if (bDefaultPath)
	{
		if (Path::IsRelativePath(InFileName))
			FileName = L"../_Textures/" + InFileName;
	}
	

	LoadMetadata(InLoadInfo);
	LoadTexture(bIsCubeMap, bUseSRGB);
	
	InFileName = Path::GetFileName(InFileName);
}

Texture::~Texture()
{
	
}

void Texture::LoadMetadata(TexMetadata& InLoadInfo)
{
	HRESULT hr;

	wstring ext = Path::GetExtension(FileName);
	if (ext == L"tga")
	{
		hr = GetMetadataFromTGAFile(FileName.c_str(), Metadata);
		Check(hr);
	}
	else if (ext == L"dds")
	{
		hr = GetMetadataFromDDSFile(FileName.c_str(), DDS_FLAGS_NONE, Metadata);
		Check(hr);
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = GetMetadataFromWICFile(FileName.c_str(), WIC_FLAGS_NONE, Metadata);
		Check(hr);
	}

	
}

void Texture::LoadTexture(bool InbIsCubeMap, bool bUseSRGB)
{
	HRESULT hr;

	ScratchImage image;

	wstring ext = Path::GetExtension(FileName);
	if (ext == L"tga")
	{
		hr = LoadFromTGAFile(FileName.c_str(), &Metadata, image);
		Check(hr);
	}
	else if (ext == L"dds")
	{
		CreateDDSTexture(InbIsCubeMap);
		return;
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		CreateTexture(bUseSRGB);
		return;
	}


	hr = CreateShaderResourceView(D3D::Get()->GetDevice(), image.GetImages(), image.GetImageCount(), Metadata, SRV.GetAddressOf());
	Check(hr);
}


void Texture::CreateDDSTexture(bool InbIsCubeMap)
{
	ComPtr<ID3D11Texture2D> texture;

	UINT miscFlags = 0;
	if (InbIsCubeMap)
	{
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	 HRESULT hr = CreateDDSTextureFromFileEx(
		D3D::Get()->GetDevice(), FileName.c_str(), 0, D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE, 0, miscFlags, DDS_LOADER_FLAGS(false),
		(ID3D11Resource**)texture.GetAddressOf(), SRV.GetAddressOf(), NULL);

	Check(hr);
}


void Texture::CreateTexture(bool bUseSRGB)
{
	int width = 0, height = 0;
	vector<uint8_t> image;
	DXGI_FORMAT pixelFormat =
		bUseSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

	string FName = String::ToString(FileName);
	string ext(FName.end() - 3, FName.end());
	std::transform(ext.begin(), ext.end(), ext.begin(),
		static_cast<int(*)(int)>(std::tolower));

	ReadImage(image, width, height);

	// 스테이징 텍스춰 만들고 CPU에서 이미지를 복사합니다.
	ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(width, height, image, pixelFormat);

	// 실제로 사용할 텍스춰 설정
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = 0; // 밉맵 레벨 최대
	txtDesc.ArraySize = 1;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용
	txtDesc.CPUAccessFlags = 0;

	// 초기 데이터 없이 텍스춰 생성 (전부 검은색)
	D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());

	// 스테이징 텍스춰로부터 가장 해상도가 높은 이미지 복사
	D3D::Get()->GetDeviceContext()->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0,
		stagingTexture.Get(), 0, NULL);

	// ResourceView 만들기
	D3D::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), 0, SRV.GetAddressOf());

	// 해상도를 낮춰가며 밉맵 생성
	D3D::Get()->GetDeviceContext()->GenerateMips(SRV.Get());

}

ComPtr<ID3D11Texture2D> Texture::CreateStagingTexture(const int width, const int height, vector<uint8_t>& image, 
	const DXGI_FORMAT pixelFormat, const int mipLevels, const int arraySize)
{
	// 스테이징 텍스춰 만들기
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = mipLevels;
	txtDesc.ArraySize = arraySize;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_STAGING;
	txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	ComPtr<ID3D11Texture2D> stagingTexture;

	if (FAILED(D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL,
		stagingTexture.GetAddressOf()))) 
	{
		cout << "Failed()" << endl;
	}

	// CPU에서 이미지 데이터 복사
	size_t pixelSize = sizeof(uint8_t) * 4;
	if (pixelFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) 
	{
		pixelSize = sizeof(uint16_t) * 4;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	D3D::Get()->GetDeviceContext()->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
	uint8_t* pData = (uint8_t*)ms.pData;
	
	for (UINT h = 0; h < UINT(height); h++) 
	{ // 가로줄 한 줄씩 복사
		memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize],
			width * pixelSize);
	}
	D3D::Get()->GetDeviceContext()->Unmap(stagingTexture.Get(), NULL);

	return stagingTexture;
}


void Texture::ReadImage(vector<uint8_t>& image, int& width, int& height)
{
	int channels;
	string filename = String::ToString(FileName);
	unsigned char* img =
		stbi_load(filename.c_str(), &width, &height, &channels, 0);

	// assert(channels == 4);

	cout << filename << " " << width << " " << height << " " << channels << endl;

	// 4채널로 만들어서 복사
	image.resize(width * height * 4);

	if (channels == 1) 
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			uint8_t g = img[i * channels + 0];
			for (size_t c = 0; c < 4; c++) 
			{
				image[4 * i + c] = g;
			}
		}
	}
	else if (channels == 2) 
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			for (size_t c = 0; c < 2; c++) 
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 2] = 255;
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 3) 
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			for (size_t c = 0; c < 3; c++) 
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 4) 
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			for (size_t c = 0; c < 4; c++) 
			{
				image[4 * i + c] = img[i * channels + c];
			}
		}
	}
	else
	{
		cout << "Cannot read " << channels << " channels" << endl;
	}
}

/*
D3D11_TEXTURE2D_DESC Texture::ReadPixel(vector<Color>& OutPixel)
{
	return ReadPixel(DXGI_FORMAT_UNKNOWN, OutPixel);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixel(DXGI_FORMAT InFormat, vector<Color>& OutPixel)
{
	ID3D11Texture2D* texture;
	SRV->GetResource((ID3D11Resource**)&texture);

	if (InFormat == DXGI_FORMAT_UNKNOWN)
	{
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);

		InFormat = desc.Format;
	}

	return ReadPixel(texture, InFormat, OutPixel);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixel(ID3D11Texture2D* InSource, DXGI_FORMAT InFormat, vector<Color>& OutPixel)
{
	D3D11_TEXTURE2D_DESC sourceDesc;
	InSource->GetDesc(&sourceDesc);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = sourceDesc.Width;
	desc.Height = sourceDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = InFormat;
	desc.SampleDesc = sourceDesc.SampleDesc;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ComPtr<ID3D11Texture2D> texture;
	Check(D3D::Get()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture));
	D3D::Get()->GetDeviceContext()->CopyResource(texture.Get(), InSource);
	
	unique_ptr<UINT[]> colors(new UINT[desc.Width * desc.Height]);
	
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::Get()->GetDeviceContext()->Map(texture.Get(), 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(colors.get(), subResource.pData, sizeof(UINT) * desc.Width * desc.Height);
	}
	D3D::Get()->GetDeviceContext()->Unmap(texture.Get(), 0);
	

	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			UINT index = desc.Width * y + x;

			OutPixel.push_back(Color(colors[index]));
		}
	}

	return desc;
}
*/