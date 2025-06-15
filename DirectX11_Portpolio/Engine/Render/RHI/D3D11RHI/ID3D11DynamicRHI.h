#pragma once
#include "Libraries/Utility/Crc.h"
#include "Render/RHI/RHIDefinition.h"
#include "Render/RHI/RHIFwd.h"
#include "Render/RHI/RHIResources.h"


class ID3D11DynamicRHI
{
public:
    virtual ERHIInterfaceType     GetInterfaceType() const { return ERHIInterfaceType::D3D11; }

    virtual ID3D11Device*         RHIGetDevice() const = 0;
    virtual ID3D11DeviceContext*  RHIGetDeviceContext() const = 0;
    virtual IDXGIAdapter*         RHIGetAdapter() const = 0;
    virtual IDXGISwapChain*       RHIGetSwapChain(FRHIViewport* InViewport) const = 0;
    virtual DXGI_FORMAT           RHIGetSwapChainFormat(EPixelFormat InFormat) const = 0;

    virtual FTexture2DRHIRef      RHICreateTexture2DFromResource(EPixelFormat Format, ETextureCreateFlags TexCreateFlags, const FClearValueBinding& ClearValueBinding, ID3D11Texture2D* Resource) = 0;
    virtual FTexture2DArrayRHIRef RHICreateTexture2DArrayFromResource(EPixelFormat Format, ETextureCreateFlags TexCreateFlags, const FClearValueBinding& ClearValueBinding, ID3D11Texture2D* Resource) = 0;
    virtual FTextureCubeRHIRef    RHICreateTextureCubeFromResource(EPixelFormat Format, ETextureCreateFlags TexCreateFlags, const FClearValueBinding& ClearValueBinding, ID3D11Texture2D* Resource) = 0;

    virtual ID3D11Buffer*         RHIGetResource(FRHIBuffer* InBuffer) const = 0;
    virtual ID3D11Resource*       RHIGetResource(FRHITexture* InTexture) const = 0;
    virtual int64                 RHIGetResourceMemorySize(FRHITexture* InTexture) const = 0;

    virtual ID3D11RenderTargetView*   RHIGetRenderTargetView(FRHITexture* InTexture, int32 InMipIndex = 0, int32 InArraySliceIndex = -1) const = 0;
    virtual ID3D11ShaderResourceView* RHIGetShaderResourceView(FRHITexture* InTexture) const = 0;

    virtual void                  RHIRegisterWork(uint32 NumPrimitives) = 0;

    virtual void                  RHIVerifyResult(ID3D11Device* Device, HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line) const = 0;

};
