#include "HeaderCollection.h"
#include "Buffers.h"

#include "Frameworks/Animation/UAnimInstance.h"
#include "Render/Resource/AnimationData.h"

VertexBuffer::VertexBuffer(void* InData, UINT InCount, UINT InStride, UINT InSlot, bool InCpuWrite, bool InGpuWrite)
	: Data(InData), Count(InCount), Stride(InStride), Slot(InSlot)
	, bCpuWrite(InCpuWrite), bGpuWrite(InGpuWrite)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = InStride * Count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (bCpuWrite == false && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bCpuWrite == true && bGpuWrite == false)
	{
		//CPU - RW(Map), GPU - R

		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (bCpuWrite == false && bGpuWrite == true)
	{
		//CPU W(UpdateSubresource), GPU - RW

		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}


	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
	subResource.pSysMem = Data;

	auto hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, Buffer.GetAddressOf());

	if (FAILED(hr)) 
	{
		std::cout << "CreateVertexBuffer() CreateVertexBuffer failed()."
			<< std::endl;
	}
}

VertexBuffer::~VertexBuffer()
{
}


void VertexBuffer::UpdateVertexBuffer()
{
	CheckFalse(bCpuWrite);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::Get()->GetDeviceContext()->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, Data, (size_t)Stride * Count);
	}
	D3D::Get()->GetDeviceContext()->Unmap(Buffer.Get(), 0);
}

void VertexBuffer::IASetVertexBuffer()
{
	UINT offset = 0;
	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(Slot, 1, Buffer.GetAddressOf(), &Stride, &offset);
}

//------------------------------------------------------------------------------

IndexBuffer::IndexBuffer(UINT* InData, UINT InCount)
	: Data(InData), Count(InCount)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(UINT) * Count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
	subResource.pSysMem = Data;

	Check(D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, Buffer.GetAddressOf()));
}


void IndexBuffer::IASetIndexBuffer() const
{
	D3D::Get()->GetDeviceContext()->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

//------------------------------------------------------------------------------

ConstantBuffer::ConstantBuffer(void* InData, UINT InDataSize)
	: Data(InData), DataSize(InDataSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	
	desc.ByteWidth = DataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	
	Check(D3D::Get()->GetDevice()->CreateBuffer(&desc, nullptr, Buffer.GetAddressOf()));
	
}


void ConstantBuffer::UpdateConstBuffer() const
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::Get()->GetDeviceContext()->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, Data, DataSize);
	}
	D3D::Get()->GetDeviceContext()->Unmap(Buffer.Get(), 0);
}

void ConstantBuffer::UpdateData(void* InData)
{
	Data = InData;
	UpdateConstBuffer();
}

void ConstantBuffer::VSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot)
{
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(static_cast<UINT>(bufferSlot), numSlot, Buffer.GetAddressOf());
}

void ConstantBuffer::PSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot)
{
	D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(static_cast<UINT>(bufferSlot), numSlot, Buffer.GetAddressOf());
}

void ConstantBuffer::CSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot)
{
	D3D::Get()->GetDeviceContext()->CSSetConstantBuffers(static_cast<UINT>(bufferSlot), numSlot, Buffer.GetAddressOf());
}


//----------------------------------------------------------------------------

StructuredBuffer::StructuredBuffer(void* inData, UINT inElementSize, UINT inElementCount)
	: Data(inData), elementSize(inElementSize), elementCount(inElementCount)
{
	if(elementSize <= 0) return;
	
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = elementSize * elementCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; // Compute Shader
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = Data;

	HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &bufferData, buffer.GetAddressOf());
	Check(hr);


	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = elementCount;
	uavDesc.Buffer.Flags =
		D3D11_BUFFER_UAV_FLAG_APPEND; // <- AppendBuffer로 사용
	Check(D3D::Get()->GetDevice()->CreateUnorderedAccessView(buffer.Get(), &uavDesc,
									  uav.GetAddressOf()));
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = elementCount;

	Check(D3D::Get()->GetDevice()->CreateShaderResourceView(buffer.Get(), &srvDesc, srv.GetAddressOf()));
}


void StructuredBuffer::UpdateBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D::Get()->GetDeviceContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	{
		memcpy(mapped.pData, Data, elementSize * elementCount);
	}
	D3D::Get()->GetDeviceContext()->Unmap(buffer.Get(), 0);
}

void StructuredBuffer::UpdateSubResource()
{
	D3D::Get()->GetDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, Data, 0, 0);
}

void StructuredBuffer::PSSetStructuredBuffer(const EShaderResourceSlot bufferSlot)
{
	D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(bufferSlot), 1, srv.GetAddressOf());
}

void StructuredBuffer::VSSetStructuredBuffer(const EShaderResourceSlot bufferSlot)
{
	D3D::Get()->GetDeviceContext()->VSSetShaderResources(static_cast<UINT>(bufferSlot), 1, srv.GetAddressOf());
}

void StructuredBuffer::UpdateData(void* InData)
{
	Data = InData;
}


//------------------------------------

IndirectBuffer::IndirectBuffer(void* Indata, UINT InelementSize, UINT InelementCount)
	:Data(Indata), elementSize(InelementSize), elementCount(InelementCount)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = elementCount * elementSize;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags =
		D3D11_BIND_UNORDERED_ACCESS; // ComputeShader에서 업데이트 가능
	desc.StructureByteStride = elementSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS; // <- IndirectArgs

	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = Data;
	if(InelementSize > 0)
	{
		D3D::Get()->GetDevice()->CreateBuffer(&desc, &bufferData, buffer.GetAddressOf());
	}
	else
	{
		D3D::Get()->GetDevice()->CreateBuffer(&desc, NULL, buffer.GetAddressOf());
	}
}

void IndirectBuffer::UpdateBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D::Get()->GetDeviceContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	{
		memcpy(mapped.pData, Data, elementSize * elementCount);
	}
	D3D::Get()->GetDeviceContext()->Unmap(buffer.Get(), 0);
}

void IndirectBuffer::UpdateSubResource()
{
	D3D::Get()->GetDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, Data, 0, 0);
}


//-----------------------------------------------------------------------------

CsResource::CsResource()
{
}

CsResource::~CsResource()
{
}


void CsResource::Create()
{
	CreateInput();
	CreateSRV();

	CreateOuput();
	CreateUAV();

	CreateResult();
}

//-----------------------------------------------------------------------------

RawBuffer::RawBuffer(void* InInputData, UINT InInputSize, UINT InOutputSize)
	: InputData(InInputData)
	, InputSize(InInputSize), OutputSize(InOutputSize)
{
	Create();
}

RawBuffer::~RawBuffer()
{

}

void RawBuffer::CreateInput()
{
	CheckFalse(InputSize > 0);


	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = InputSize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
	subResource.pSysMem = InputData;

	Check(D3D::Get()->GetDevice()->CreateBuffer(&desc, InputData != nullptr ? &subResource : nullptr, &buffer));

	Input = (ID3D11Resource*)buffer;
}

void RawBuffer::CreateSRV()
{
	CheckFalse(InputSize > 0);

	ID3D11Buffer* buffer = (ID3D11Buffer*)Input.Get();

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;

	Check(D3D::Get()->GetDevice()->CreateShaderResourceView(buffer, &srvDesc, SRV.GetAddressOf()));
}

void RawBuffer::CreateOuput()
{
	Assert(OutputSize > 0, "잘못된 OutputSize");

	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = OutputSize;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.Usage = D3D11_USAGE_DEFAULT;

	Check(D3D::Get()->GetDevice()->CreateBuffer(&desc, nullptr, &buffer));

	Output = (ID3D11Resource*)buffer;
}

void RawBuffer::CreateUAV()
{
	Assert(OutputSize > 0, "잘못된 OutputSize");

	ID3D11Buffer* buffer = (ID3D11Buffer*)Output.Get();

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	uavDesc.Buffer.NumElements = desc.ByteWidth / 4;

	Check(D3D::Get()->GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, UAV.GetAddressOf()));
}

void RawBuffer::CreateResult()
{
	Assert(OutputSize > 0, "잘못된 OutputSize");

	D3D11_BUFFER_DESC desc;
	((ID3D11Buffer*)Output.Get())->GetDesc(&desc);

	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ID3D11Buffer* buffer = nullptr;
	Check(D3D::Get()->GetDevice()->CreateBuffer(&desc, nullptr, &buffer));

	Result = (ID3D11Resource*)buffer;
}

void RawBuffer::SetInputData(void* InData)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	Check(D3D::Get()->GetDeviceContext()->Map(Input.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource));
	{
		memcpy(subResource.pData, InData, InputSize);
	}
	D3D::Get()->GetDeviceContext()->Unmap(Input.Get(), 0);
}

void RawBuffer::GetOutputData(void* OutData)
{
	D3D::Get()->GetDeviceContext()->CopyResource(Result.Get(), Output.Get());

	D3D11_MAPPED_SUBRESOURCE subResource;
	Check(D3D::Get()->GetDeviceContext()->Map(Result.Get(), 0, D3D11_MAP_READ, 0, &subResource));
	{
		memcpy(OutData, subResource.pData, OutputSize);
	}
	D3D::Get()->GetDeviceContext()->Unmap(Result.Get(), 0);
}

//-----------------------------------------------------------------------------

TextureBuffer::TextureBuffer(ID3D11Texture2D* InSource)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	InSource->GetDesc(&srcDesc);

	Width = srcDesc.Width;
	Height = srcDesc.Height;
	ArraySize = srcDesc.ArraySize;
	Format = srcDesc.Format;


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = Width;
	desc.Height = Height;
	desc.ArraySize = ArraySize;
	desc.Format = Format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture = nullptr;
	Check(D3D::Get()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture));
	D3D::Get()->GetDeviceContext()->CopyResource(texture, InSource);

	Input = (ID3D11Resource*)texture;


	Create();
}

TextureBuffer::~TextureBuffer()
{
}

void TextureBuffer::CreateSRV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)Input.Get();

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
	srvDesc.Texture2DArray.ArraySize = ArraySize;

	Check(D3D::Get()->GetDevice()->CreateShaderResourceView(texture, &srvDesc, SRV.GetAddressOf()));
}

void TextureBuffer::CreateOuput()
{
	ID3D11Texture2D* texture = nullptr;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = Width;
	desc.Height = Height;
	desc.ArraySize = ArraySize;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	//desc.Usage = D3D11_USAGE_DEFAULT;

	Check(D3D::Get()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture));

	Output = (ID3D11Resource*)texture;
}

void TextureBuffer::CreateUAV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)Output.Get();

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = ArraySize;

	Check(D3D::Get()->GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, UAV.GetAddressOf()));
}

void TextureBuffer::CreateResult()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)Output.Get();

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = ArraySize;

	Check(D3D::Get()->GetDevice()->CreateShaderResourceView(texture, &srvDesc, OutputSRV.GetAddressOf()));
}

AppendBuffer::AppendBuffer(void* Indata, UINT InelementSize, UINT InelementCount)
	:Data(Indata), elementSize(InelementSize), elementCount(InelementCount)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = InelementCount * InelementSize;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | // Compute Shader
						   D3D11_BIND_SHADER_RESOURCE;   // Vertex Shader
	bufferDesc.StructureByteStride = InelementSize;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	if (Indata)
	{
		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = Indata;
		FAILED(D3D::Get()->GetDevice()->CreateBuffer(&bufferDesc, &bufferData,
										   buffer.GetAddressOf()));
	}
	else
	{
		FAILED(D3D::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, buffer.GetAddressOf()));
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = InelementCount;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND; // <- AppendBuffer로 사용
	FAILED(D3D::Get()->GetDevice()->CreateUnorderedAccessView(buffer.Get(), &uavDesc,
									  UAV.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.NumElements = InelementCount;
	FAILED(D3D::Get()->GetDevice()->CreateShaderResourceView(buffer.Get(), &srvDesc,
									 SRV.GetAddressOf()));
}

void AppendBuffer::CSSetUAV(const EUAV_Slot bufferSlot, UINT InitCount)
{
	D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(static_cast<UINT>(bufferSlot), 1, UAV.GetAddressOf(), &InitCount);
}

void AppendBuffer::CSSetSRV(const EShaderResourceSlot bufferSlot)
{
	D3D::Get()->GetDeviceContext()->CSSetShaderResources(static_cast<UINT>(bufferSlot), 1, SRV.GetAddressOf());
}

void AppendBuffer::CSClearUAV(const EUAV_Slot bufferSlot)
{
	D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(static_cast<UINT>(bufferSlot), 1, nullptr, 0);
}

void AppendBuffer::CSClearSRV(const EShaderResourceSlot bufferSlot)
{
	D3D::Get()->GetDeviceContext()->CSSetShaderResources(static_cast<UINT>(bufferSlot), 0, nullptr);
}

void AppendBuffer::VSSetSRV(const EShaderResourceSlot bufferSlot)
{
	D3D::Get()->GetDeviceContext()->VSSetShaderResources(static_cast<UINT>(bufferSlot), 1, SRV.GetAddressOf());

}

void AppendBuffer::SwapBuffer(AppendBuffer& InBuffer)
{
	std::swap(buffer, InBuffer.buffer);
    std::swap(SRV, InBuffer.SRV);
    std::swap(UAV, InBuffer.UAV);
    std::swap(Data, InBuffer.Data);
    std::swap(elementSize, InBuffer.elementSize);
    std::swap(elementCount, InBuffer.elementCount);
}

void AppendBuffer::UpdateSubResource()
{
	D3D::Get()->GetDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, Data, 0, 0);
}

void AnimationTexture::CreateAnimationTexture(USkeletalMeshComponent* meshComp,
	ComPtr<ID3D11Texture2D>& InClipTexture, ComPtr<ID3D11ShaderResourceView>& InClipSRV)
{
	vector<shared_ptr<FClipData::ClipTransform>> v;

	vector<shared_ptr<FClipData>> animations = meshComp->GetAnimInstance()->Animations;
	
	for (shared_ptr<FClipData>& anim : animations)
		v.push_back(anim->CalcClipTransform(meshComp->Bones));

	
	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORM * 4; //Bone
		desc.Height = MAX_MODEL_KEYFRAME; //Frame
		desc.ArraySize = animations.size(); //Clip
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16Byte * 4 = 64 Byte
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		
		UINT pageSize = MAX_MODEL_TRANSFORM * 4 * 16 * MAX_MODEL_KEYFRAME;
		void* p = VirtualAlloc(nullptr, pageSize * animations.size(), MEM_RESERVE, PAGE_READWRITE);
		

		for (UINT c = 0; c < animations.size(); c++)
		{
			UINT start = c * pageSize;

			for (UINT f = 0; f < MAX_MODEL_KEYFRAME; f++)
			{
				//if (f > Animations[c]->Duration)
					//break;

				void* temp = (BYTE*)p + MAX_MODEL_TRANSFORM * f * sizeof(Matrix) + start;

				VirtualAlloc(temp, MAX_MODEL_TRANSFORM * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, v[c]->Transform[f], MAX_MODEL_TRANSFORM * sizeof(Matrix));
			}
		}

		
		//SSD -> VRAM
		{
			D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[animations.size()];
			for (UINT c = 0; c < animations.size(); c++)
			{
				void* temp = (BYTE*)p + c * pageSize;

				subResource[c].pSysMem = temp;
				subResource[c].SysMemPitch = MAX_MODEL_TRANSFORM * sizeof(Matrix);
				subResource[c].SysMemSlicePitch = pageSize;
			}
			Check(D3D::Get()->GetDevice()->CreateTexture2D(&desc, subResource, InClipTexture.GetAddressOf()));

			DeleteArray(subResource);
		}
		
		//Clear
		{
			VirtualFree(p, 0, MEM_RELEASE);
		}

		//Create SRV
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipLevels = 1;
			desc.Texture2DArray.ArraySize = animations.size();

			Check(D3D::Get()->GetDevice()->CreateShaderResourceView(InClipTexture.Get(), &desc, InClipSRV.GetAddressOf()));
		}
	}
}


ComPtr<ID3D11Texture2D> TextureBuffer::CreateStagingTexture( const std::vector<uint8_t> &image,
                                                             const int width, const int height, const DXGI_FORMAT pixelFormat, const int mipLevels, const int arraySize)
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
	if (FAILED(D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL, stagingTexture.GetAddressOf())))
	{
		cout << "Failed()" << endl;
	}
	
	size_t pixelSize = GetPixelSize(pixelFormat);

	D3D11_MAPPED_SUBRESOURCE ms;
	D3D::Get()->GetDeviceContext()->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
	uint8_t *pData = (uint8_t *)ms.pData;
	for (UINT h = 0; h < UINT(height); h++) { // 가로줄 한 줄씩 복사
		memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize],
			   width * pixelSize);
	}
	D3D::Get()->GetDeviceContext()->Unmap(stagingTexture.Get(), NULL);

	return stagingTexture;
}

void TextureBuffer::CreateUAVTexture(const int width, const int height, const DXGI_FORMAT pixelFormat,
	ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11RenderTargetView>& rtv, ComPtr<ID3D11ShaderResourceView>& srv,
	ComPtr<ID3D11UnorderedAccessView>& uav)
{
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = 1;
	txtDesc.ArraySize = 1;
	txtDesc.Format = pixelFormat; // 주로 FLOAT 사용
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	txtDesc.MiscFlags = 0;
	txtDesc.CPUAccessFlags = 0;

	
	FAILED(D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf()));
	FAILED(D3D::Get()->GetDevice()->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf()));
	FAILED(D3D::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf()));
	FAILED(D3D::Get()->GetDevice()->CreateUnorderedAccessView(texture.Get(), NULL, uav.GetAddressOf()));
}


void TextureBuffer::CreateBuffer(ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& srv,
	ComPtr<ID3D11RenderTargetView>& rtv, int width, int height)
{
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = txtDesc.ArraySize = 1;
	txtDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //  이미지 처리용도
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	txtDesc.MiscFlags = 0;
	txtDesc.CPUAccessFlags = 0;


	D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
	D3D::Get()->GetDevice()->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf());
	D3D::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf());
}


size_t CsResource::GetPixelSize(DXGI_FORMAT pixelFormat)
{

	switch (pixelFormat)
	{
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return sizeof(uint16_t) * 4;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return sizeof(uint32_t) * 4;
	case DXGI_FORMAT_R32_FLOAT:
		return sizeof(uint32_t) * 1;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return sizeof(uint8_t) * 4;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return sizeof(uint8_t) * 4;
	case DXGI_FORMAT_R32_SINT:
		return sizeof(int32_t) * 1;
	case DXGI_FORMAT_R16_FLOAT:
		return sizeof(uint16_t) * 1;
	}

	cout << "PixelFormat not implemented " << pixelFormat << endl;

	return sizeof(uint8_t) * 4;
}
