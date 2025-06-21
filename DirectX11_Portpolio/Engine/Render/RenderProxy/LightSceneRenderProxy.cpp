#include "HeaderCollection.h"
#include "LightSceneRenderProxy.h"

LightSceneRenderProxy::LightSceneRenderProxy(FSceneView* sceneView)
    :RenderProxy(ERenderProxyType::RPT_LightScene)
{
    Data.LightInformation = sceneView->LightsCBuffer;
    Data.IBLSRVRef = reinterpret_cast<ID3D11ShaderResourceView**>(sceneView->IBLSRV);
}


void LightSceneRenderProxy::Render(const FRenderOption& option)
{
    Data.LightInformation->UpdateConstBuffer();
    Data.LightInformation->PSSetConstantBuffer(EConstBufferSlot::LightInfo, 1);
    
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture),
        static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT), Data.IBLSRVRef);
}
