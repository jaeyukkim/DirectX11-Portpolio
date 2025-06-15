#include "HeaderCollection.h"
#include "ViewRenderProxy.h"

ViewRenderProxy::ViewRenderProxy(FSceneView* sceneView)
    :RenderProxy(ERenderProxyType::RPT_View)
{
    Data.ViewConstantBuffer = sceneView->ViewConstantBuffer;
    Data.ReflactViewConstantBuffer = sceneView->ReflactViewConstantBuffer;
}


void ViewRenderProxy::Render(const FRenderOption& option)
{
    if(option.bIsMirror)
    {
        Data.ReflactViewConstantBuffer->UpdateConstBuffer();
        Data.ReflactViewConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);
        Data.ReflactViewConstantBuffer->PSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);
    }
    else
    {
        Data.ViewConstantBuffer->UpdateConstBuffer();
        Data.ViewConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);
        Data.ViewConstantBuffer->PSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);
    }
}
