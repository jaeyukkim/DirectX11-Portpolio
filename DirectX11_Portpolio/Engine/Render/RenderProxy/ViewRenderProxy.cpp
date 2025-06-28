#include "HeaderCollection.h"
#include "ViewRenderProxy.h"

ViewRenderProxy::ViewRenderProxy(FSceneView* sceneView)
    :RenderProxy(ERenderProxyType::RPT_View)
{
    Data.ViewConstantBuffer = sceneView->ViewConstantBuffer;
    Data.ReflactViewConstantBuffer = sceneView->ReflactViewConstantBuffer;
    
    for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
    {
        Data.LightViewConstantBuffer[i] = sceneView->LightViewConstantBuffer[i];
    }
}


void ViewRenderProxy::Render(const FRenderOption& option)
{
    if(option.bIsMirror)
    {
        Data.ReflactViewConstantBuffer->UpdateConstBuffer();
        Data.ReflactViewConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
        Data.ReflactViewConstantBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
    }
    else
    {
        Data.ViewConstantBuffer->UpdateConstBuffer();
        Data.ViewConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
        Data.ViewConstantBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
    }
}

void ViewRenderProxy::SetLightViewMode(UINT8 InLightID)
{
    if(InLightID >= MAX_LIGHT_COUNT) return;
    Data.LightViewConstantBuffer[InLightID]->UpdateConstBuffer();
    Data.LightViewConstantBuffer[InLightID]->VSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
    Data.LightViewConstantBuffer[InLightID]->PSSetConstantBuffer(EConstBufferSlot::CB_ViewContext, 1);
}
