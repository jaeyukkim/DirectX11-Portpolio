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

    vector<LightInformation> lights(MAX_LIGHT_COUNT);
    lights.assign(MAX_LIGHT_COUNT, LightInformation{});
    
    LightCntCbuffer.CurrentLightCnt = 0;
    for (auto& [id, info] : LightMap)
    {
        if (info.LightID == InLightInfo.LightID)
        {
            info = InLightInfo;   //라이트 업데이트
            lights[info.LightID] = info;  //업데이트 할 라이트에 추가
            LightCntCbuffer.CurrentLightCnt++;
        }
    }

    // StructuredBuffer에 라이트 배열 업로드
    LightConstantBuffer->UpdateData(lights.data());
    LightConstantBuffer->PSSetStructuredBuffer(EConstBufferSlot::LightMap); // t5에 바인딩

    LightCountCBuffer->UpdateConstBuffer();
    LightCountCBuffer->PSSetConstantBuffer(EConstBufferSlot::LightCnt, 1);
}


