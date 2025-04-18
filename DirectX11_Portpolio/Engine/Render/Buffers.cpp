#include "HeaderCollection.h"
#include "Buffers.h"

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

	Check(D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, Buffer.GetAddressOf()));
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

	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(Slot, 1, &Buffer, &Stride, &offset);
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

void ConstantBuffer::VSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot)
{
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(static_cast<UINT>(bufferSlot), numSlot, Buffer.GetAddressOf());
}

void ConstantBuffer::PSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot)
{
	D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(static_cast<UINT>(bufferSlot), numSlot, Buffer.GetAddressOf());

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
