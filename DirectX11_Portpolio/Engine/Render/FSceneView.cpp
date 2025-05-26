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
    Instance->ReflactViewConstantBuffer = make_shared<ConstantBuffer>(&Instance->ReflactContext, sizeof(Instance->ReflactContext));
    Instance->LightCountCBuffer = make_shared<ConstantBuffer>(&Instance->LightInfoCbuffer, sizeof(Instance->LightInfoCbuffer));
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
    Instance->ViewConstantBuffer->PSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);

    D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture),
        static_cast<int>(CubeMapType::MAX_CUBEMAP_TEXTURE_COUNT), Instance->IBLSRV->GetAddressOf());

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
    ReflactContext.ReflectRow = InReflactRow.Transpose();
    ReflactContext.View = (InReflactRow * Context.View.Transpose()).Transpose();
    ReflactContext.ViewProjection = (InReflactRow * Context.View.Transpose() * Context.Projection.Transpose()).Transpose();

    Instance->ReflactViewConstantBuffer->UpdateConstBuffer();
    Instance->ReflactViewConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);
    Instance->ReflactViewConstantBuffer->PSSetConstantBuffer(EConstBufferSlot::ViewContext, 1);
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
    LightConstantBuffer->PSSetStructuredBuffer(EShaderResourceSlot::LightMap);

    // ����Ʈ ���� ������Ʈ (LightMap ũ�� ����)
    LightInfoCbuffer.CurrentLightCnt = static_cast<UINT32>(LightMap.size());
    LightCountCBuffer->UpdateConstBuffer();
    LightCountCBuffer->PSSetConstantBuffer(EConstBufferSlot::LightCnt, 1);
}


void FSceneView::UpdateSkyLight(CubeMapType IBLType, ID3D11ShaderResourceView* InIBLSRV)
{
    Assert(InIBLSRV != nullptr, "UpdateSkyLight FAILED");

    int IBLTypeToIdx = static_cast<int>(IBLType);
    IBLSRV[IBLTypeToIdx] = InIBLSRV;
   
}
