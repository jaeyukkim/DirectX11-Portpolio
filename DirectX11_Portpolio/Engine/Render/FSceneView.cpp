#include "HeaderCollection.h"
#include "FSceneView.h"

FSceneView* FSceneView::Instance = nullptr;

FSceneView* FSceneView::Get()
{
    if (!Instance)
        return nullptr;
   
    return Instance;
}

void FSceneView::Create()
{
    Assert(Instance == nullptr, "SceneView Create ½ÇÆÐ");

    Instance = new FSceneView();
    CheckNull(Instance);

    Instance->ViewConstantBuffer = make_shared<ConstantBuffer>(&Instance->Context, sizeof(Instance->Context));
}

void FSceneView::Destroy()
{
    Delete(Instance);
}

void FSceneView::PreRender()
{
    CheckNull(Instance);

    Instance->ViewConstantBuffer->UpdateConstBuffer();
    Instance->ViewConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);

}


void FSceneView::UpdateSceneView(const Matrix& Inview, const Matrix& Inprojection)
{
    Context.View = Inview; 
    Context.ViewInverse = Inview.Invert();
    Context.Projection = Inprojection;
    Context.ViewProjection = Context.View * Context.Projection;
}
