#include "HeaderCollection.h"
#include "Texture.h"

using namespace DirectX;

Texture::Texture(wstring InFileName, TexMetadata& InLoadInfo, bool bDefaultPath)
{
	FileName = InFileName;

	if (bDefaultPath)
	{
		if (Path::IsRelativePath(InFileName))
			FileName = L"../_Textures/" + InFileName;
	}
	

	LoadMetadata(InLoadInfo);
	LoadTexture();
	
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

void Texture::LoadTexture()
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
		hr = LoadFromDDSFile(FileName.c_str(), DDS_FLAGS_NONE, &Metadata, image);
		Check(hr);
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = LoadFromWICFile(FileName.c_str(), WIC_FLAGS_NONE, &Metadata, image);
		Check(hr);
	}

	
	hr = DirectX::CreateShaderResourceView(D3D::Get()->GetDevice(), image.GetImages(), image.GetImageCount(), Metadata, &SRV);
	Check(hr);
}

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
	//Check(D3DX11LoadTextureFromTexture(D3D::Get()->GetDeviceContext(), InSource, nullptr, texture));
	D3D::Get()->GetDeviceContext()->CopyResource(texture.Get(), InSource);

	//UINT* colors = new UINT[desc.Width * desc.Height];
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
