#include "HeaderCollection.h"
#include "FSceneView.h"

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
    Instance->LightCountCBuffer = make_shared<ConstantBuffer>(&Instance->LightCntCbuffer, sizeof(Instance->LightCntCbuffer));
    Instance->LightConstantBuffer = make_shared<StructuredBuffer>(nullptr, sizeof(LightInformation), MAX_LIGHT_COUNT);
    Instance->CachedLights.resize(MAX_LIGHT_COUNT, LightInformation{});
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


void FSceneView::UpdateSceneView(const FViewContext& InContext)
{
    Context.View = InContext.View.Transpose(); 
    Context.ViewInverse = InContext.ViewInverse.Transpose();
    Context.Projection = InContext.Projection.Transpose();
    Context.ViewProjection = InContext.ViewProjection.Transpose();
    Context.EyePos = InContext.EyePos;
    
}

void FSceneView::AddToLightMap(LightInformation* InLightInfo)
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
        Assert(false, "Light count exceeded MAX_LIGHT_COUNT!");
        return;
    }

    InLightInfo->LightID = LightCounter.load(); // 라이트 ID부여
    LightMap[LightCounter] = *InLightInfo; // 라이트맵에 등록
    
    LightCounter++;
}

void FSceneView::UpdateLightMap(LightInformation& InLightInfo)
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

    // GPU에 업로드 (전체 전송)
    LightConstantBuffer->UpdateData(CachedLights.data());
    LightConstantBuffer->PSSetStructuredBuffer(EConstBufferSlot::LightMap);

    // 라이트 개수 업데이트 (LightMap 크기 기준)
    LightCntCbuffer.CurrentLightCnt = static_cast<UINT32>(LightMap.size());
    LightCountCBuffer->UpdateConstBuffer();
    LightCountCBuffer->PSSetConstantBuffer(EConstBufferSlot::LightCnt, 1);
}


