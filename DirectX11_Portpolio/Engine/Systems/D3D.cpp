#include "HeaderCollection.h"
#include "Render/PostProcess/PostProcess.h"
#include "D3D.h"

D3D* D3D::Instance = nullptr;

D3DDesc D3D::D3dDesc = D3DDesc();

D3D* D3D::Get()
{
	return Instance;
}

void D3D::Create()
{
	assert(Instance == nullptr);

	Instance = new D3D();
	Instance->CreateDevice();
	Instance->CreateRTV();
	Instance->CreateDSV();
	Instance->CreateViewport();
	Instance->CreatePostProcess();
	
}

void D3D::Destroy()
{
	Delete(Instance);
}


const D3DDesc& D3D::GetDesc()
{
	return D3dDesc;
}

void D3D::SetDesc(const D3DDesc& InDesc)
{
	D3dDesc = InDesc;
}


void D3D::RunPostProcess()
{
	postProcess->Render();
}

void D3D::SetFloatRTV()
{
	DeviceContext->OMSetRenderTargets(1, FloatRTV.GetAddressOf(), DepthStencilView.Get());
}

void D3D::SetRenderTarget()
{
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
}

void D3D::ClearDSV()
{
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D::ClearOnlyDepth()
{
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::ClearRTV()
{
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
}

void D3D::ClearFloatRTV()
{
	DeviceContext->ClearRenderTargetView(FloatRTV.Get(), clearColor);

}

void D3D::ClearBlendState()
{
	DeviceContext->OMSetBlendState(nullptr, NULL, 0xffffffff);
}


void D3D::Present()
{
	SwapChain->Present(1, 0);
}

void D3D::EndDraw()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	assert(SUCCEEDED(hr) && "SwapChain 백버퍼 가져오기 실패");

	DeviceContext->CopyResource(RenderTargetBuffer.Get(), backBuffer.Get());
}

void D3D::ResizeScreen(float InWidth, float InHeight)
{
	if (InWidth < 1 || InHeight < 1)
		return;

	D3dDesc.Width = InWidth;
	D3dDesc.Height = InHeight;

	RenderTargetView->Release();
	{
		SwapChain->ResizeBuffers(0, (UINT)InWidth, (UINT)InHeight, DXGI_FORMAT_UNKNOWN, 0);
	}
	CreateRTV();
	CreateViewport();
	CreateDSV();
	CreatePostProcess();

	WinSizeChanged.Broadcast();
}

D3D::D3D()
	:clearColor(Color(0.5f, 0.5f, 0.5f, 1.0f))
{
	
}

D3D::~D3D()
{
	
}

void D3D::CreateDevice()
{
	//Create Device And SwapChain
	{
		DXGI_MODE_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_MODE_DESC));

		desc.Width = (UINT)D3dDesc.Width;
		desc.Height = (UINT)D3dDesc.Height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.RefreshRate.Numerator = 200;
		desc.RefreshRate.Denominator = 1;
		

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDesc.BufferDesc = desc;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		swapChainDesc.OutputWindow = D3dDesc.Handle;

		
		HRESULT hr = D3D11CreateDeviceAndSwapChain
		(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			SwapChain.GetAddressOf(),
			Device.GetAddressOf(),
			nullptr,
			DeviceContext.GetAddressOf()
		);
		assert(hr >= 0 && "Device 생성 실패");
	}
}

void D3D::CreateRTV()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	assert(hr >= 0 && "Back Buffer 가져오기 실패");

	hr = Device->CreateRenderTargetView(backBuffer.Get(), nullptr, RenderTargetView.GetAddressOf());
	assert(hr >= 0 && "RTV 생성 실패");

	assert(RenderTargetView != nullptr);

	FAILED(Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &NumQualityLevels));

	D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc(&desc);
	desc.MipLevels = desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스쳐로부터 복사 가능
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	if (bUseMSAA && NumQualityLevels)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = NumQualityLevels - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}

	
	// FLOAT MSAA 적용 된 RTV, SRV
	FAILED(Device->CreateTexture2D(&desc, NULL, FloatBuffer.GetAddressOf()));
	FAILED(Device->CreateShaderResourceView(FloatBuffer.Get(), NULL, FloatSRV.GetAddressOf()));
	FAILED(Device->CreateRenderTargetView(FloatBuffer.Get(), NULL, FloatRTV.GetAddressOf()));


	// FLOAT MSAA를 Relsolve해서 저장할 SRV/RTV
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	FAILED(Device->CreateTexture2D(&desc, NULL, ResolvedBuffer.GetAddressOf()));
	FAILED(Device->CreateShaderResourceView(ResolvedBuffer.Get(), NULL, ResolvedSRV.GetAddressOf()));
	FAILED(Device->CreateRenderTargetView(ResolvedBuffer.Get(), NULL, ResolvedRTV.GetAddressOf()));

	
	//PostEffect용 SRV/RTV
	FAILED(Device->CreateTexture2D(&desc, NULL, PostEffectsBuffer.GetAddressOf()));
	FAILED(Device->CreateShaderResourceView(PostEffectsBuffer.Get(), NULL, PostEffectSRV.GetAddressOf()));
	FAILED(Device->CreateRenderTargetView(PostEffectsBuffer.Get(), NULL, PostEffectRTV.GetAddressOf()));


	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	FAILED(Device->CreateTexture2D(&desc, NULL, RenderTargetBuffer.GetAddressOf()));
	FAILED(Device->CreateShaderResourceView(RenderTargetBuffer.Get(), NULL, RenderTargetSRV.GetAddressOf()));

}


void D3D::CreateViewport()
{
	{
		Viewport = make_shared<D3D11_VIEWPORT>();
		ZeroMemory(Viewport.get(), sizeof(D3D11_VIEWPORT));
		Viewport->TopLeftX = 0;
		Viewport->TopLeftY = 0;
		Viewport->Width = D3dDesc.Width;
		Viewport->Height = D3dDesc.Height;
		Viewport->MinDepth = 0;
		Viewport->MaxDepth = 1;
	}

	
	{
		ShadowViewport =  make_shared<D3D11_VIEWPORT>();
		ZeroMemory(ShadowViewport.get(), sizeof(D3D11_VIEWPORT));
		ShadowViewport->TopLeftX = 0;
		ShadowViewport->TopLeftY = 0;
		ShadowViewport->Width = ShadowWidth;
		ShadowViewport->Height = ShadowHeight;
		ShadowViewport->MinDepth = 0;
		ShadowViewport->MaxDepth = 1;
	}

	
	DeviceContext->RSSetViewports(1, Viewport.get());
	
}


void D3D::CreateDSV()
{
	
	//Create Texture - DSV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)D3dDesc.Width;
		desc.Height = (UINT)D3dDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (NumQualityLevels > 0)
		{
			desc.SampleDesc.Count = 4; 
			desc.SampleDesc.Quality = NumQualityLevels - 1;
		}
		else 
		{
			desc.SampleDesc.Count = 1; 
			desc.SampleDesc.Quality = 0;
		}

		Check(Device->CreateTexture2D(&desc, nullptr, DSV_Texture.GetAddressOf()));

		// Depth 전용
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		Check(Device->CreateTexture2D(&desc, nullptr, DepthOnlyBuffer.GetAddressOf()));
	}
	
	//Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (NumQualityLevels > 0)
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; //  MSAA용
		else
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		Check(Device->CreateDepthStencilView(DSV_Texture.Get(), &desc, DepthStencilView.GetAddressOf()));
	}
	
	//DepthOnlyDSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		Check(Device->CreateDepthStencilView(DepthOnlyBuffer.Get(), &desc, DepthOnlyDSV.GetAddressOf()));
	}

	//DepthOnlySRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		Check(Device->CreateShaderResourceView(DepthOnlyBuffer.Get(), &desc, DepthOnlySRV.GetAddressOf()));
	}
	
}


void D3D::CreatePostProcess()
{
	postProcess = make_shared<PostProcess>(
		vector<ComPtr<ID3D11ShaderResourceView>>{ PostEffectSRV },
		vector<ComPtr<ID3D11RenderTargetView>>{ RenderTargetView });
}

void D3D::CreateShadowResources(UINT8 InLightID)
{
	FShadowMapResources sdwResource;
	
	//Create Texture - ShadowTexture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = ShadowWidth;
		desc.Height = ShadowHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1; 
		desc.SampleDesc.Quality = 0;
		

		Check(Device->CreateTexture2D(&desc, nullptr, sdwResource.ShadowBuffer.GetAddressOf()));
	}

	
	//ShadowDSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		Check(Device->CreateDepthStencilView(sdwResource.ShadowBuffer.Get(), &desc, sdwResource.ShadowDSV.GetAddressOf()));
	}

	//ShadowSRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		Check(Device->CreateShaderResourceView(sdwResource.ShadowBuffer.Get(), &desc, sdwResource.ShadowSRV.GetAddressOf()));
	}

	ShadowResources.insert({InLightID, sdwResource});
}

void D3D::DeleteShadowResource(UINT8 InLightID)
{
	ShadowResources.erase(InLightID);
}
