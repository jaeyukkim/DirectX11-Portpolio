#include "HeaderCollection.h"
#include "ViewRenderProxy.h"

ViewRenderProxy::ViewRenderProxy(FSceneView* sceneView)
    :RenderProxy(ERenderProxyType::RPT_View)
{
    Data.ViewCBuffer = sceneView->ViewCBuffer;
    Data.ReflectViewCBuffer = sceneView->ReflactViewCBuffer;
    Data.FrustumCBuffer = sceneView->FrustumCBuffer;
    Data.ReflectFrustumCBuffer = sceneView->FrustumCBuffer;
    
    for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
    {
        Data.LightViewCBuffer[i] = sceneView->LightViewCBuffer[i];
    }
    
}


void ViewRenderProxy::Render(const FRenderOption& option)
{
    if(option.bIsMirror)
    {
        Data.ReflectViewCBuffer->UpdateConstBuffer();
        Data.ReflectViewCBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
        Data.ReflectViewCBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
        Data.ReflectFrustumCBuffer->CSSetConstantBuffer(EConstBufferSlot::CB_FrustumData, 1);
    }
    else
    {
        Data.ViewCBuffer->UpdateConstBuffer();
        Data.ViewCBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
        Data.ViewCBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
        Data.FrustumCBuffer->CSSetConstantBuffer(EConstBufferSlot::CB_FrustumData, 1);
    }
}

void ViewRenderProxy::SetLightViewMode(UINT8 InLightID)
{
    if(InLightID >= MAX_LIGHT_COUNT) return;
    Data.LightViewCBuffer[InLightID]->UpdateConstBuffer();
    Data.LightViewCBuffer[InLightID]->VSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
    Data.LightViewCBuffer[InLightID]->PSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
}
