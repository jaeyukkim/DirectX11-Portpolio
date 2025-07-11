#pragma once

struct FShadowMapResources;
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
	static void Create();
	static void Destroy();

	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);
	
	FWinSizeChangedSignature WinSizeChanged;



public:
	ID3D11Device* GetDevice() { return Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext.Get(); }
	IDXGISwapChain* GetSwapChain() { return SwapChain.Get(); }

	ID3D11ShaderResourceView* GetRenderTargetSRV() { return RenderTargetSRV.Get(); }
	PostProcess* GetPostProcess() { return postProcess.get(); }
	
	void CreateShadowResources(UINT8 InLightID);
	void DeleteShadowResource(UINT8 InLightID);
	void RunPostProcess();
	void SetFloatRTV();
	void SetRenderTarget();
	void ClearDSV();
	void ClearOnlyDepth();
	void ClearRTV();
	void ClearFloatRTV();
	void ClearBlendState();
	void Present();
	void EndDraw();
	void ComputeShaderBarrier();

	void ResizeScreen(float InWidth, float InHeight);
	


public:
	ComPtr<ID3D11Texture2D> DSV_Texture;
	ComPtr<ID3D11DepthStencilView> DepthStencilView;

	// Depth buffer 관련
	ComPtr<ID3D11Texture2D> DepthOnlyBuffer; // No MSAA
	ComPtr<ID3D11DepthStencilView> DepthOnlyDSV;
	ComPtr<ID3D11ShaderResourceView> DepthOnlySRV;

	// 기본 RenderTarget
	ComPtr<ID3D11Texture2D> RenderTargetBuffer;
	ComPtr<ID3D11RenderTargetView> RenderTargetView;
	ComPtr<ID3D11ShaderResourceView> RenderTargetSRV;
	
	

	//Resolved
	ComPtr<ID3D11Texture2D> ResolvedBuffer;
	ComPtr<ID3D11ShaderResourceView> ResolvedSRV;
	ComPtr<ID3D11RenderTargetView> ResolvedRTV;

	//Float Buffer
	ComPtr<ID3D11Texture2D> FloatBuffer;
	ComPtr<ID3D11RenderTargetView> FloatRTV;
	ComPtr<ID3D11ShaderResourceView> FloatSRV;

	//PostEffect
	ComPtr<ID3D11Texture2D> PostEffectsBuffer;
	ComPtr<ID3D11RenderTargetView> PostEffectRTV;
	ComPtr<ID3D11ShaderResourceView> PostEffectSRV;

	unordered_map<UINT8, FShadowMapResources> ShadowResources;
	
	//PostProcess
	shared_ptr<PostProcess> postProcess;

	shared_ptr<D3D11_VIEWPORT> Viewport;
	shared_ptr<D3D11_VIEWPORT> ShadowViewport;

private:
	D3D();
	~D3D();
	void CreateDevice();
	void CreateRTV();
	void CreateViewport();
	void CreateDSV();
	void CreatePostProcess();

	
private:
	static D3D* Instance;
	static D3DDesc D3dDesc;
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D11Device> Device;
	ComPtr<ID3D11DeviceContext> DeviceContext;
	

	
private:
	UINT NumQualityLevels = 0;
	bool bUseMSAA = true;
	const UINT ShadowWidth = 1280;
	const UINT ShadowHeight = 1280;
	Color clearColor;
};