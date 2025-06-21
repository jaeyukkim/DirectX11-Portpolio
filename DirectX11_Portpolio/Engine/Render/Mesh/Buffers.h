#pragma once


class VertexBuffer
{
public:
	VertexBuffer(void* InData, UINT InCount, UINT InStride, UINT InSlot = 0, bool InCpuWrite = false, bool InGpuWrite = false);
	~VertexBuffer();

	operator ID3D11Buffer* () { return Buffer.Get(); }
	operator const ID3D11Buffer* () { return Buffer.Get(); }

	UINT GetCount() { return Count; }
	UINT GetStride() { return Stride; }

	void UpdateVertexBuffer();
	void IASetVertexBuffer();

private:
	ComPtr<ID3D11Buffer> Buffer = nullptr;

	void* Data;
	UINT Count;
	UINT Stride;
	UINT Slot;

private:
	bool bCpuWrite;
	bool bGpuWrite;
};

//------------------------------------------------------------------------------

class IndexBuffer
{
public:
	IndexBuffer(UINT* InData, UINT InCount);


	UINT GetCount() const { return Count; } 

	void IASetIndexBuffer() const;

private:
	ComPtr<ID3D11Buffer> Buffer;

	UINT* Data;
	UINT Count;
};

//------------------------------------------------------------------------------

class ConstantBuffer
{
public:
	ConstantBuffer(void* InData, UINT InDataSize);

	operator ID3D11Buffer* () { return Buffer.Get(); }
	operator const ID3D11Buffer* () { return Buffer.Get(); }

	void UpdateConstBuffer() const;
	void UpdateData(void* InData);
	void VSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot);
	void PSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot);
private:
	ComPtr<ID3D11Buffer> Buffer;

	void* Data;
	UINT DataSize;
};


//------------------------------------------------------------------------


class StructuredBuffer
{
public:
	StructuredBuffer(void* data, UINT elementSize, UINT elementCount);
	~StructuredBuffer() = default;

	void UpdateBuffer();
	void PSSetStructuredBuffer(const EShaderResourceSlot bufferSlot);
	void VSSetStructuredBuffer(const EShaderResourceSlot bufferSlot);
	void UpdateData(void* InData);

private:
	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> srv;

	void* data = nullptr;
	UINT elementSize = 0;
	UINT elementCount = 0;
};

//------------------------------------------------------------------------------

class CsResource
{
public:
	CsResource();
	virtual ~CsResource();

public:
	operator ID3D11UnorderedAccessView* () { return UAV.Get(); }
	operator const ID3D11UnorderedAccessView* () { return UAV.Get(); }

	operator ID3D11ShaderResourceView* () { return SRV.Get(); }
	operator const ID3D11ShaderResourceView* () { return SRV.Get(); }

protected:
	virtual void CreateInput() {}
	virtual void CreateSRV() = 0;

	virtual void CreateOuput() = 0;
	virtual void CreateUAV() = 0;

	virtual void CreateResult() = 0;

protected:
	void Create();

protected:
	ComPtr<ID3D11Resource> Input = nullptr;
	ComPtr<ID3D11ShaderResourceView> SRV = nullptr;

	ComPtr<ID3D11Resource> Output = nullptr;
	ComPtr<ID3D11UnorderedAccessView> UAV = nullptr;

	ComPtr<ID3D11Resource> Result = nullptr;
};

//------------------------------------------------------------------------------

class RawBuffer : public CsResource
{
public:
	RawBuffer(void* InInputData, UINT InInputSize, UINT InOutputSize);
	virtual ~RawBuffer() override;

private:
	void CreateInput() override;
	void CreateSRV() override;
	void CreateOuput() override;
	void CreateUAV() override;
	void CreateResult() override;

public:
	void SetInputData(void* InData);
	void GetOutputData(void* OutData);

private:
	void* InputData = nullptr;

	UINT InputSize = 0;
	UINT OutputSize = 0;
};

//------------------------------------------------------------------------------

class TextureBuffer : public CsResource
{
public:
	TextureBuffer(ID3D11Texture2D* InSource);
	virtual ~TextureBuffer() override;

private:
	void CreateSRV() override;
	void CreateOuput() override;
	void CreateUAV() override;
	void CreateResult() override;

public:
	UINT GetWidth() { return Width; }
	UINT GetHeight() { return Height; }
	UINT GetArraySize() { return ArraySize; }
	DXGI_FORMAT GetFormat() { return Format; }

	ID3D11Texture2D* GetOutput() { return (ID3D11Texture2D*)Output.Get(); }
	ID3D11ShaderResourceView* GetOutputSRV() { return OutputSRV.Get(); }

private:
	UINT Width;
	UINT Height;
	UINT ArraySize;
	DXGI_FORMAT Format;

	ComPtr<ID3D11ShaderResourceView> OutputSRV;
};