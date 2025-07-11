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
	void CSSetConstantBuffer(const EConstBufferSlot bufferSlot, const int numSlot);

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
	void UpdateSubResource();
	void PSSetStructuredBuffer(const EShaderResourceSlot bufferSlot);
	void VSSetStructuredBuffer(const EShaderResourceSlot bufferSlot);
	void UpdateData(void* InData);

private:
	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;

	void* Data = nullptr;
	UINT elementSize = 0;
	UINT elementCount = 0;
};

//----------------------------------------------------------------------------

class IndirectBuffer
{
public:
	IndirectBuffer(void* Indata, UINT InelementSize, UINT InelementCount);
	~IndirectBuffer() = default;

	void UpdateBuffer();
	void UpdateSubResource();
	ComPtr<ID3D11Buffer> GetBuffer() {return buffer;}
private:
	ComPtr<ID3D11Buffer> buffer;
	void* Data = nullptr;
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
	static size_t GetPixelSize(DXGI_FORMAT pixelFormat);
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
	
	static ComPtr<ID3D11Texture2D> CreateStagingTexture(const std::vector<uint8_t> &image,
		const int width, const int height, const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		const int mipLevels = 1, const int arraySize = 1);

	static void CreateUAVTexture(const int width,
								const int height,
								const DXGI_FORMAT pixelFormat,
								ComPtr<ID3D11Texture2D> &texture,
								ComPtr<ID3D11RenderTargetView> &rtv,
								ComPtr<ID3D11ShaderResourceView> &srv,
								ComPtr<ID3D11UnorderedAccessView> &uav);

	static void CreateBuffer(ComPtr<ID3D11Texture2D>& texture,
		ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv,
		int width, int height);
	
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

//--------------------------------------------------------------------------
class AppendBuffer
{
public:
	AppendBuffer(void* Indata, UINT InelementSize, UINT InelementCount);
	~AppendBuffer() = default;

	void CSSetUAV(const EUAV_Slot bufferSlot, UINT InitCount = 0);
	void CSSetSRV(const EShaderResourceSlot bufferSlot);

	void CSClearUAV(const EUAV_Slot bufferSlot);
	void CSClearSRV(const EShaderResourceSlot bufferSlot);

	void VSSetSRV(const EShaderResourceSlot bufferSlot);

	void SwapBuffer(AppendBuffer& InBuffer);
	void UpdateSubResource();

	ComPtr<ID3D11Buffer> GetBuffer() {return buffer;}
	ComPtr<ID3D11ShaderResourceView> GetSRV() {return SRV;}
	ComPtr<ID3D11UnorderedAccessView> GetUAV() {return UAV;}
	
private:
	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> SRV;
	ComPtr<ID3D11UnorderedAccessView> UAV;

	void* Data = nullptr;
	UINT elementSize = 0;
	UINT elementCount = 0;
};