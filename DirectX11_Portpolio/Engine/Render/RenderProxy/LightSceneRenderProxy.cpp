#include "HeaderCollection.h"
#include "LightSceneRenderProxy.h"

LightSceneRenderProxy::LightSceneRenderProxy(FSceneView* sceneView)
    :RenderProxy(ERenderProxyType::RPT_LightScene)
{
    Data.LightsCBuffer = sceneView->LightsCBuffer;
    Data.LightInfoCBuffer = sceneView->LightsInfoCBuffer;
    Data.IBLSRVRef = reinterpret_cast<ID3D11ShaderResourceView**>(sceneView->IBLSRV);
}


void LightSceneRenderProxy::Render(const FRenderOption& option)
{
    Data.LightsCBuffer->UpdateConstBuffer();
    Data.LightsCBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_LightObjects, 1);

    Data.LightInfoCBuffer->UpdateConstBuffer();
    Data.LightInfoCBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_LightInfo, 1);
    
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::ERS_CubeMapTexture),
        static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT), Data.IBLSRVRef);
}
