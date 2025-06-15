#include "HeaderCollection.h"
#include "FSceneView.h"

#include "Render/FSceneRender.h"

FSceneView* FSceneView::Instance = nullptr;
atomic<uint8_t> FSceneView::LightCounter = 0;

FSceneView* FSceneView::Get()
{
    if (!Instance)
        return nullptr;
   
    return Instance;
}


void FSceneView::Create()
{
    Assert(Instance == nullptr, "SceneView Create 실패");

    Instance = new FSceneView();
    CheckNull(Instance);

    Instance->ViewConstantBuffer = make_shared<ConstantBuffer>(&Instance->Context, sizeof(Instance->Context));
    Instance->ReflactViewConstantBuffer = make_shared<ConstantBuffer>(&Instance->ReflactContext, sizeof(Instance->ReflactContext));
    Instance->LightCountCBuffer = make_shared<ConstantBuffer>(&Instance->LightInfoCbuffer, sizeof(Instance->LightInfoCbuffer));
    Instance->LightConstantBuffer = make_shared<StructuredBuffer>(Instance->CachedLights.data(), sizeof(FLightInformation), MAX_LIGHT_COUNT);
    Instance->CachedLights.resize(MAX_LIGHT_COUNT, FLightInformation{});

    FSceneRender::Get()->CreateRenderProxy<LightSceneRenderProxy>(Instance);
    FSceneRender::Get()->CreateRenderProxy<ViewRenderProxy>(Instance);

}


void FSceneView::Destroy()
{
    Delete(Instance);
}



void FSceneView::UpdateSceneView(const FViewContext& InContext)
{
    Context.View = InContext.View.Transpose(); 
    Context.ViewInverse = InContext.ViewInverse.Transpose();
    Context.Projection = InContext.Projection.Transpose();
    Context.ViewProjection = InContext.ViewProjection.Transpose();
    Context.EyePos = InContext.EyePos;
}

void FSceneView::UpdateReflactRow(const Matrix InReflactRow)
{
    ReflactContext = Context;
    ReflactContext.View = (InReflactRow * Context.View.Transpose()).Transpose();
    ReflactContext.ViewProjection = (InReflactRow * Context.View.Transpose() * Context.Projection.Transpose()).Transpose();
    Instance->ReflactViewConstantBuffer->UpdateConstBuffer();
}

void FSceneView::AddToLightMap(FLightInformation* InLightInfo)
{
    for (const auto& [id, info] : LightMap)
    {
        if (info.LightID == InLightInfo->LightID)
        {
            return; // 이미 등록한 라이트의 경우 무시
        }
    }

    // 최대 라이트 수를 초과할경우
   
    if (LightCounter.load() >= MAX_LIGHT_COUNT)
    {
        Assert(true, "Light count exceeded MAX_LIGHT_COUNT!");
        return;
    }

    InLightInfo->LightID = LightCounter.load(); // 라이트 ID부여
    LightMap[LightCounter] = *InLightInfo; // 라이트맵에 등록
    
    ++LightCounter;
}

void FSceneView::UpdateLightMap(FLightInformation& InLightInfo)
{
    CheckFalse(LightMap.size());

    // LightMap에서 해당 ID만 업데이트
    auto it = LightMap.find(InLightInfo.LightID);
    if (it != LightMap.end())
    {
        it->second = InLightInfo;

        // CachedLights에서도 해당 ID만 업데이트
        if (InLightInfo.LightID >= 0 && InLightInfo.LightID < CachedLights.size())
        {
            CachedLights[InLightInfo.LightID] = InLightInfo;
        }
    }

    LightConstantBuffer->UpdateData(CachedLights.data());
    // 라이트 개수 업데이트 (LightMap 크기 기준)
    LightInfoCbuffer.CurrentLightCnt = static_cast<UINT32>(LightMap.size());
    
}


void FSceneView::UpdateSkyLight(ECubeMapType IBLType, ID3D11ShaderResourceView* InIBLSRV)
{
    Assert(InIBLSRV != nullptr, "UpdateSkyLight FAILED");

    int IBLTypeToIdx = static_cast<int>(IBLType);
    IBLSRV[IBLTypeToIdx] = InIBLSRV;
}