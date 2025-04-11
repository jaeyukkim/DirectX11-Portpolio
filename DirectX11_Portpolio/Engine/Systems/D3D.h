#pragma once

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;

	float Width;
	float Height;

	float Background[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

DECLARE_DYNAMIC_DELEGATE(FTestDelegate);


class D3D
{
public:
	static D3D* Get();

public:
	static void Create();
	static void Destroy();

	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);

	FTestDelegate testDel;

private:
	void CreateDevice();
	void CreateRTV();
	void CreateViewport();

private:
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

public:
	ID3D11Device* GetDevice() { return Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext.Get(); }

	void ClearRenderTargetView();
	void Present();

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

	ComPtr<ID3D11RenderTargetView> RenderTargetView;
	shared_ptr<D3D11_VIEWPORT> Viewport;
};