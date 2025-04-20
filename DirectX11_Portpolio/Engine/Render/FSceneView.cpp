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

    vector<LightInformation> lights(MAX_LIGHT_COUNT);
    lights.assign(MAX_LIGHT_COUNT, LightInformation{});
    
    LightCntCbuffer.CurrentLightCnt = 0;
    for (auto& [id, info] : LightMap)
    {
        if (info.LightID == InLightInfo.LightID)
        {
            info = InLightInfo;   //����Ʈ ������Ʈ
            lights[info.LightID] = info;  //������Ʈ �� ����Ʈ�� �߰�
            LightCntCbuffer.CurrentLightCnt++;
        }
    }

    // StructuredBuffer�� ����Ʈ �迭 ���ε�
    LightConstantBuffer->UpdateData(lights.data());
    LightConstantBuffer->PSSetStructuredBuffer(EConstBufferSlot::LightMap); // t5�� ���ε�

    LightCountCBuffer->UpdateConstBuffer();
    LightCountCBuffer->PSSetConstantBuffer(EConstBufferSlot::LightCnt, 1);
}


