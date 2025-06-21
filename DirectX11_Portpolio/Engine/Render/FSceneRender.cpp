#include "HeaderCollection.h"
#include "FSceneRender.h"
#include "PostEffects/PostEffect.h"


FSceneRender* FSceneRender::Instance = nullptr;

void FSceneRender::Create()
{
    assert(Instance == nullptr);
    Instance = new FSceneRender();
    FGlobalPSO::Create();
    Instance->PostEffectEntity = make_shared<PostEffect>();
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

void FSceneRender::Render()
{
    BeginRender();
    RenderDepthOnly();
    RenderShadowMap();
    D3D::Get()->SetFloatRTV();


    RenderObjects(GetDefaultRenderType());
    RenderMirror();
    EndRender();
}

void FSceneRender::BeginRender()
{
    D3D::Get()->ClearFloatRTV();
    D3D::Get()->ClearRTV();
    D3D::Get()->ClearDSV();
    D3D::Get()->ClearBlendState();
    D3D::Get()->SetFloatRTV();

}

void FSceneRender::RenderDepthOnly()
{
    D3D::Get()->GetDeviceContext()->OMSetRenderTargets(1, D3D::Get()->ResolvedRTV.GetAddressOf(),
                                  D3D::Get()->DepthOnlyDSV.Get());
    D3D::Get()->GetDeviceContext()->ClearDepthStencilView(D3D::Get()->DepthOnlyDSV.Get(), D3D11_CLEAR_DEPTH,
                                     1.0f, 0);
    
    FRenderOption option = GetDefaultRenderType();
    ViewProxy->Render(option);
    LightSceneProxy->Render(option);

    FRenderOption defaultOption = GetDefaultRenderType();
    RenderObjects(defaultOption);
    for(shared_ptr<MirrorRenderProxy> mirror : MirrorProxy)
    {
        mirror->Render(defaultOption);
    }

}

void FSceneRender::RenderShadowMap()
{
    D3D::Get()->GetDeviceContext()->RSSetViewports(1, D3D::Get()->ShadowViewport.get());
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->DepthOnlyPSO);
    
    for(FLightInformation& info : FSceneView::Get()->GetLightInfo()->Lights)
    {
        int id = info.LightID;
        if(info.LightType & LT_UseShadow)
        {
            auto it = D3D::Get()->ShadowResources.find(id);
            if (it != D3D::Get()->ShadowResources.end())
            {
                auto& shadow = it->second;
                D3D::Get()->GetDeviceContext()->OMSetRenderTargets(0, nullptr, shadow.ShadowDSV.Get());
                D3D::Get()->GetDeviceContext()->ClearDepthStencilView(shadow.ShadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
                ViewProxy->SetLightViewMode(id);
            }

            FRenderOption noOption = FRenderOption();
            RenderObjects(noOption);
            for(shared_ptr<MirrorRenderProxy> mirror : MirrorProxy)
            {
                mirror->Render(noOption);
            }
        }
    }

    D3D::Get()->GetDeviceContext()->RSSetViewports(1, D3D::Get()->Viewport.get());  //다시복구
    ViewProxy->Render(GetDefaultRenderType());
}


void FSceneRender::RenderObjects(FRenderOption option)
{
    for(shared_ptr<StaticMeshRenderProxy> proxy: MeshProxies)
    {
        proxy->Render(option);
    }
    for(shared_ptr<SkeletalMeshRenderProxy> proxy : SkeletalMeshProxies)
    {
        proxy->Render(option);
    }
    SkyBoxProxy->Render(option);
}


void FSceneRender::RenderMirror()
{
    FRenderOption mirrorOption = GetMirrorRenderType();
    for(shared_ptr<MirrorRenderProxy> mirror : MirrorProxy)
    {
        mirror->Render(GetStencilRenderType());
        D3D::Get()->ClearOnlyDepth();

        //뷰 타입을 mirror로 변경
        FSceneView::Get()->UpdateReflactView(mirror->GetReflactRow());
        ViewProxy->Render(mirrorOption);
        RenderObjects(mirrorOption);
        
        ViewProxy->Render(GetDefaultRenderType());
        mirror->Render(GetBlendRenderType()); //거울의 재질 블랜드
    }
}

void FSceneRender::EndRender()
{
    D3D::Get()->GetDeviceContext()->ResolveSubresource(D3D::Get()->ResolvedBuffer.Get(), 0,
        D3D::Get()->FloatBuffer.Get(), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

    PostEffectEntity->Render();
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

FRenderOption FSceneRender::GetDepthOnlyRenderType()
{
    FRenderOption option;

    option.bDepthOnly = true;
    return option;
}
