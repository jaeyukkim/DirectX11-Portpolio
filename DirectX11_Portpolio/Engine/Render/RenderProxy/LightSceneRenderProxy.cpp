#include "HeaderCollection.h"
#include "LightSceneRenderProxy.h"

LightSceneRenderProxy::LightSceneRenderProxy(FSceneView* sceneView)
    :RenderProxy(ERenderProxyType::RPT_LightScene)
{
    Data.LightCount = sceneView->LightCountCBuffer;
    Data.LightInformation = sceneView->LightConstantBuffer;
    Data.IBLSRVRef = reinterpret_cast<ID3D11ShaderResourceView**>(sceneView->IBLSRV);
    
}


void LightSceneRenderProxy::Render(const FRenderOption& option)
{
    Data.LightInformation->UpdateBuffer();
    Data.LightInformation->PSSetStructuredBuffer(EShaderResourceSlot::LightMap);

    Data.LightCount->UpdateConstBuffer();
    Data.LightCount->PSSetConstantBuffer(EConstBufferSlot::LightCnt, 1);
    
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture),
        static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT), Data.IBLSRVRef);
}
