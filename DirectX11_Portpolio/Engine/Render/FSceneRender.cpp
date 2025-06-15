#include "HeaderCollection.h"
#include "FSceneRender.h"


FSceneRender* FSceneRender::Instance = nullptr;

void FSceneRender::Create()
{
    assert(Instance == nullptr);
    Instance = new FSceneRender();
    FGlobalPSO::Create();
}

void FSceneRender::Destroy()
{
    assert(Instance != nullptr);
  
    FGlobalPSO::Destroy();
    Delete(Instance);
}

FSceneRender* FSceneRender::Get()
{
    Assert(Instance != nullptr, "FSceneView NotCreated");
    return Instance;
}


void FSceneRender::BeginRender()
{
    D3D::Get()->ClearFloatRTV();
    D3D::Get()->ClearRTV();
    D3D::Get()->ClearDSV();
    D3D::Get()->ClearBlendState();
    D3D::Get()->SetFloatRTV();
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->RenderPSO);
    FRenderOption option = GetDefaultRenderType();
    ViewProxy->Render(option);
    LightSceneProxy->Render(option);
  
}

void FSceneRender::Render()
{
    BeginRender();


    //기본 렌더링
    FRenderOption defaultOption = GetDefaultRenderType();
    for(const shared_ptr<StaticMeshRenderProxy>& proxy : MeshProxies)
    {
        proxy->Render(defaultOption);
    }
    for(const shared_ptr<SkeletalMeshRenderProxy>& proxy : SkeletalMeshProxies)
    {
        proxy->Render(defaultOption);
    }
    SkyBoxProxy->Render(defaultOption);


    //거울 렌더링
    FRenderOption mirrorOption = GetMirrorRenderType();
    for(shared_ptr<MirrorRenderProxy> mirror : MirrorProxy)
    {
        mirror->Render(GetStencilRenderType());
        D3D::Get()->ClearOnlyDepth();

        //뷰 타입을 mirror로 변경
        FSceneView::Get()->UpdateReflactRow(mirror->GetReflactRow());
        ViewProxy->Render(mirrorOption);

        for(shared_ptr<StaticMeshRenderProxy> proxy: MeshProxies)
        {
            proxy->Render(mirrorOption);
        }
        for(shared_ptr<SkeletalMeshRenderProxy> proxy : SkeletalMeshProxies)
        {
            proxy->Render(mirrorOption);
        }
        SkyBoxProxy->Render(mirrorOption);

        
        ViewProxy->Render(defaultOption);
        mirror->Render(GetBlendRenderType()); //거울의 재질 블랜드
    }

    EndRender();
}

void FSceneRender::EndRender()
{
    D3D::Get()->RunPostProcess();
    D3D::Get()->EndDraw();
}

FRenderOption FSceneRender::GetDefaultRenderType()
{
    FRenderOption option;
    if(bWireRender == false)
        option.bDefaultDraw = true;
    else
        option.bIsWire = true;

   
    return option;
}

FRenderOption FSceneRender::GetMirrorRenderType()
{
    FRenderOption option;
    if (bWireRender == false)
        option.bDefaultDraw = true;
    else
        option.bIsWire = true;
    
    option.bIsMirror = true;
    return option;
}

FRenderOption FSceneRender::GetStencilRenderType()
{
    FRenderOption option;
    
    option.bStencilOn = true;
    return option;
}

FRenderOption FSceneRender::GetBlendRenderType()
{
    FRenderOption option;
    
    option.bBlendOn = true;
    return option;
}