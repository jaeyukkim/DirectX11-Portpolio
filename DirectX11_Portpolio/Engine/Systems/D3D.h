#pragma once

class PostProcess;

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;

	float Width;
	float Height;

	float Background[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

DECLARE_DYNAMIC_DELEGATE(FWinSizeChangedSignature);


class D3D
{
public:
	static D3D* Get();

public:
	static void Create();
	static void Destroy();

	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);
	

	FWinSizeChangedSignature WinSizeChanged;

private:
	void CreateDevice();
	void CreateRTV();
	void CreateViewport();
	void CreateDSV();
	void CreatePostProcess();

private:
	Color clearColor;

public:
	ID3D11Device* GetDevice() { return Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext.Get(); }
	ID3D11ShaderResourceView* GetRenderTargetSRV() { return RenderTargetSRV.Get(); }
	PostProcess* GetPostProcess() { return postProcess.get(); }


	void RunPostProcess();
	void SetFloatRTV();
	void SetRenderTarget();
	void ClearDSV();
	void ClearRTV();
	void ClearFloatRTV();
	void ClearBlendState();
	void Present();
	void EndDraw();

	void ResizeScreen(float InWidth, float InHeight);
	
private:
	D3D();
	~D3D();

private:
	static D3D* Instance;
	static D3DDesc D3dDesc;

	

private:
	ComPtr<IDXGISwapChain> SwapChain;

	ComPtr<ID3D11Device> Device;
	ComPtr<ID3D11DeviceContext> DeviceContext;

	
	shared_ptr<D3D11_VIEWPORT> Viewport;

	ComPtr<ID3D11Texture2D> DSV_Texture;
	ComPtr<ID3D11DepthStencilView> DepthStencilView;

private:
	ComPtr<ID3D11Texture2D> FloatBuffer;
	ComPtr<ID3D11ShaderResourceView> FloatSRV;
	ComPtr<ID3D11RenderTargetView> FloatRTV;

	ComPtr<ID3D11Texture2D> ResolvedBuffer;
	ComPtr<ID3D11ShaderResourceView> ResolvedSRV;
	ComPtr<ID3D11RenderTargetView> ResolvedRTV;
	
	ComPtr<ID3D11Texture2D> RenderTargetBuffer;
	ComPtr<ID3D11ShaderResourceView> RenderTargetSRV;
	ComPtr<ID3D11RenderTargetView> RenderTargetView;

	shared_ptr<PostProcess> postProcess;

private:
	UINT NumQualityLevels = 0;
	bool bUseMSAA = true;
	
};