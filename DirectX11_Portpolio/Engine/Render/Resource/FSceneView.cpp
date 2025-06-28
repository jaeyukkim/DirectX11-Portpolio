#include "HeaderCollection.h"
#include "FSceneView.h"

#include "Render/FSceneRender.h"

FSceneView* FSceneView::Instance = nullptr;
atomic<uint8_t> FSceneView::LightCounter = 0;
atomic<uint8_t> FSceneView::ShadowCounter = 0;

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

    for(int i = 0 ; i < MAX_LIGHT_COUNT ; i++)
    {
        Instance->Lights.Lights[i] = FLight();
        Instance->ShadowView[i] = FViewContext();
        Instance->LightViewConstantBuffer[i] = make_shared<ConstantBuffer>(&Instance->ShadowView[i], sizeof(FViewContext));
    }
    
    Instance->ViewConstantBuffer = make_shared<ConstantBuffer>(&Instance->DefaultView, sizeof(FViewContext));
    Instance->ReflactViewConstantBuffer = make_shared<ConstantBuffer>(&Instance->ReflactView, sizeof(FViewContext));
    
    

    Instance->LightsCBuffer = make_shared<ConstantBuffer>(&Instance->Lights, sizeof(FLightObjects));
    Instance->LightsInfoCBuffer = make_shared<ConstantBuffer>(&Instance->LightInfo, sizeof(FLightInfo));
    


    FSceneRender::Get()->CreateRenderProxy<LightSceneRenderProxy>(Instance);
    FSceneRender::Get()->CreateRenderProxy<ViewRenderProxy>(Instance);
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        D3D::Get()->CreateShadowResources(i);
    }
}


void FSceneView::Destroy()
{
    Delete(Instance);
}



void FSceneView::UpdateSceneView(const FViewContext& InContext)
{
    DefaultView.View = InContext.View.Transpose(); 
    DefaultView.ViewInverse = InContext.ViewInverse.Transpose();
    DefaultView.Projection = InContext.Projection.Transpose();
    DefaultView.ProjectionInverse = InContext.ProjectionInverse.Transpose();
    DefaultView.ViewProjection = InContext.ViewProjection.Transpose();
    DefaultView.EyePos = InContext.EyePos;
}

void FSceneView::UpdateReflactView(const Matrix InReflactRow)
{
    ReflactView = DefaultView;
    ReflactView.View = (InReflactRow * DefaultView.View.Transpose()).Transpose();
    ReflactView.ViewProjection = (InReflactRow * DefaultView.View.Transpose() * DefaultView.Projection.Transpose()).Transpose();
    Instance->ReflactViewConstantBuffer->UpdateConstBuffer();
}

void FSceneView::AddToLightMap(FLight* InLightInfo)
{
    
    // 최대 라이트 수를 초과할경우
    if (LightCounter.load() >= MAX_LIGHT_COUNT)
    {
        Assert(true, "Light count exceeded MAX_LIGHT_COUNT!");
        return;
    }
    
    //빈 라이트 공간 찾아서 넣음
    for(int i = 0 ; i<MAX_LIGHT_COUNT ; i++)
    {
        if(Lights.Lights[i].LightType == ELightType::LT_None)
        {
            InLightInfo->LightID = i;
            Lights.Lights[i] = *InLightInfo; // 라이트맵에 등록
            LightInfo.CurrentLightCnt = ++LightCounter;
            LightInfo.ShadowCount = ++ShadowCounter;

            if(InLightInfo->LightType & LT_UseShadow)
            {
                UpdateLightView(InLightInfo);
                D3D::Get()->CreateShadowResources(InLightInfo->LightID);
            }
            
            break;
        }
    }
    
}


void FSceneView::DeleteFromLightMap(int InLightID)
{
    if(InLightID >= MAX_LIGHT_COUNT)
        return;

    if(Lights.Lights[InLightID].LightType & LT_UseShadow)
    {
        D3D::Get()->DeleteShadowResource(InLightID);
        LightInfo.ShadowCount = --ShadowCounter;
    }
    
    Lights.Lights[InLightID] = FLight();
    LightInfo.CurrentLightCnt = --LightCounter;
    ShadowView[InLightID] = FViewContext();
}


void FSceneView::UpdateLightMap(FLight* InLightInfo)
{
    if(InLightInfo->LightID >= MAX_LIGHT_COUNT)
        return;

    Lights.Lights[InLightInfo->LightID] = *InLightInfo;
    LightInfo.CurrentLightCnt = LightCounter;
    LightInfo.ShadowCount = ShadowCounter;
    UpdateLightView(InLightInfo);
}


void FSceneView::UpdateSkyLight(ECubeMapType IBLType, ID3D11ShaderResourceView* InIBLSRV)
{
    Assert(InIBLSRV != nullptr, "UpdateSkyLight FAILED");

    int IBLTypeToIdx = static_cast<int>(IBLType);
    IBLSRV[IBLTypeToIdx] = InIBLSRV;
}


/**
 * Light가 Shadow를 사용한다면 그 LightID에 맞는 Light시점 뷰 생성하여 ShadowView[LightID]에 저장
 */
void FSceneView::UpdateLightView(FLight* InLightInfo)
{
    if(InLightInfo->LightID >= MAX_LIGHT_COUNT) return;
    
    if(InLightInfo->LightType & LT_UseShadow)
    {
        
        Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
        if (abs(up.Dot(InLightInfo->direction) + 1.0f) < 1e-5)
            up = Vector3(1.0f, 0.0f, 0.0f);

        // 그림자맵을 만들 때 필요
        Matrix lightViewRow = XMMatrixLookAtLH(
            InLightInfo->position, InLightInfo->position + InLightInfo->direction, up);

        Matrix lightProjRow = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(75.0f), 1.0f, 1.0f, 50.0f);

        
        auto& view = ShadowView[InLightInfo->LightID];

        view.EyePos = InLightInfo->position;
        view.View = lightViewRow.Transpose();
        view.Projection = lightProjRow.Transpose();
        view.ProjectionInverse =
            lightProjRow.Invert().Transpose();
        view.ViewProjection =
            (lightViewRow * lightProjRow).Transpose();

        InLightInfo->invProj = view.ProjectionInverse;
        InLightInfo->viewProj = view.ViewProjection;


        /*
        // LIGHT_FRUSTUM_WIDTH 확인
         Vector4 eye(0.0f, 0.0f, 0.0f, 1.0f);
         Vector4 xLeft(-1.0f, -1.0f, 0.0f, 1.0f);
         Vector4 xRight(1.0f, 1.0f, 0.0f, 1.0f);
         eye = Vector4::Transform(eye, lightProjRow);
         xLeft = Vector4::Transform(xLeft, lightProjRow.Invert());
         xRight = Vector4::Transform(xRight, lightProjRow.Invert());
         xLeft /= xLeft.w;
         xRight /= xRight.w;
         auto light_frustum_width =  xRight.x - xLeft.x;
        cout << light_frustum_width;*/
    }
    
}
