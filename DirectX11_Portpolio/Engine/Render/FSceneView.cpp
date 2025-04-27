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
    Assert(Instance == nullptr, "SceneView Create ����");

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
            return; // �̹� ����� ����Ʈ�� ��� ����
        }
    }

    // �ִ� ����Ʈ ���� �ʰ��Ұ��
   
    if (LightCounter.load() >= MAX_LIGHT_COUNT)
    {
        Assert(false, "Light count exceeded MAX_LIGHT_COUNT!");
        return;
    }

    InLightInfo->LightID = LightCounter.load(); // ����Ʈ ID�ο�
    LightMap[LightCounter] = *InLightInfo; // ����Ʈ�ʿ� ���
    
    LightCounter++;
}

void FSceneView::UpdateLightMap(LightInformation& InLightInfo)
{
    CheckFalse(LightMap.size());

    // LightMap���� �ش� ID�� ������Ʈ
    auto it = LightMap.find(InLightInfo.LightID);
    if (it != LightMap.end())
    {
        it->second = InLightInfo;

        // CachedLights������ �ش� ID�� ������Ʈ
        if (InLightInfo.LightID >= 0 && InLightInfo.LightID < CachedLights.size())
        {
            CachedLights[InLightInfo.LightID] = InLightInfo;
        }
    }

    // GPU�� ���ε� (��ü ����)
    LightConstantBuffer->UpdateData(CachedLights.data());
    LightConstantBuffer->PSSetStructuredBuffer(EConstBufferSlot::LightMap);

    // ����Ʈ ���� ������Ʈ (LightMap ũ�� ����)
    LightCntCbuffer.CurrentLightCnt = static_cast<UINT32>(LightMap.size());
    LightCountCBuffer->UpdateConstBuffer();
    LightCountCBuffer->PSSetConstantBuffer(EConstBufferSlot::LightCnt, 1);
}


