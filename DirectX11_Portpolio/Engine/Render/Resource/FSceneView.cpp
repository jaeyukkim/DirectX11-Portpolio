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
    Assert(Instance == nullptr, "SceneView Create ����");

    Instance = new FSceneView();
    CheckNull(Instance);

    for(int i = 0 ; i < MAX_LIGHT_COUNT ; i++)
    {
        Instance->Lights.Lights[i] = FLight();
        Instance->ShadowView[i] = FViewContext();
        Instance->LightViewCBuffer[i] = make_shared<ConstantBuffer>(&Instance->ShadowView[i], sizeof(FViewContext));
    }
    
    Instance->ViewCBuffer = make_shared<ConstantBuffer>(&Instance->DefaultView, sizeof(FViewContext));
    Instance->ReflactViewCBuffer = make_shared<ConstantBuffer>(&Instance->ReflactView, sizeof(FViewContext));
    
    

    Instance->LightsCBuffer = make_shared<ConstantBuffer>(&Instance->Lights, sizeof(FLightObjects));
    Instance->LightsInfoCBuffer = make_shared<ConstantBuffer>(&Instance->LightInfo, sizeof(FLightInfo));
    

    Instance->Frustum.resize(6);
    Instance->FrustumCBuffer = make_shared<ConstantBuffer>(nullptr, sizeof(XMVECTOR)*6);

    Instance->ReflectFrustum.resize(6);
    Instance->ReflectFrustumCBuffer = make_shared<ConstantBuffer>(nullptr, sizeof(XMVECTOR)*6);
    

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
    Instance->ReflactViewCBuffer->UpdateConstBuffer();

    ReflactFrustumView = FrustumView;
    ReflactFrustumView.View = InReflactRow * ReflactFrustumView.View;
    ReflactFrustumView.ViewProjection = (InReflactRow * ReflactFrustumView.View * ReflactFrustumView.Projection);
    CreateFrustum(ReflectFrustum, ReflactFrustumView.ViewProjection, ReflectFrustumCBuffer);
    
}

void FSceneView::AddToLightMap(FLight* InLightInfo)
{
    
    // �ִ� ����Ʈ ���� �ʰ��Ұ��
    if (LightCounter.load() >= MAX_LIGHT_COUNT)
    {
        Assert(true, "Light count exceeded MAX_LIGHT_COUNT!");
        return;
    }
    
    //�� ����Ʈ ���� ã�Ƽ� ����
    for(int i = 0 ; i<MAX_LIGHT_COUNT ; i++)
    {
        if(Lights.Lights[i].LightType == ELightType::LT_None)
        {
            InLightInfo->LightID = i;
            Lights.Lights[i] = *InLightInfo; // ����Ʈ�ʿ� ���
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
 * Light�� Shadow�� ����Ѵٸ� �� LightID�� �´� Light���� �� �����Ͽ� ShadowView[LightID]�� ����
 */
void FSceneView::UpdateLightView(FLight* InLightInfo)
{
    if(InLightInfo->LightID >= MAX_LIGHT_COUNT) return;
    
    if(InLightInfo->LightType & LT_UseShadow)
    {
        
        Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
        if (abs(up.Dot(InLightInfo->direction) + 1.0f) < 1e-5)
            up = Vector3(1.0f, 0.0f, 0.0f);

        // �׸��ڸ��� ���� �� �ʿ�
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
        // LIGHT_FRUSTUM_WIDTH Ȯ��
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

void FSceneView::CreateFrustum(const FViewContext& InContext)
{
    FrustumView = InContext;
    CreateFrustum(Frustum, FrustumView.ViewProjection, FrustumCBuffer);
}

void FSceneView::CreateFrustum(vector<XMVECTOR>& InFrustum, const Matrix& viewProj,
                                const shared_ptr<ConstantBuffer>& InFrustumCBuffer)
{
    // 1) NDC �������� 8�� ������ ���� (Direct3D: x,y in [-1,1], z in [0,1])
    XMVECTOR ndcCorners[8] =
    {
        XMVectorSet(-1, -1, 0, 1), // near bottom left
        XMVectorSet(1, -1, 0, 1), // near bottom right
        XMVectorSet(-1,  1, 0, 1), // near top left
        XMVectorSet(1,  1, 0, 1), // near top right
        XMVectorSet(-1, -1, 1, 1), // far bottom left
        XMVectorSet(1, -1, 1, 1), // far bottom right
        XMVectorSet(-1,  1, 1, 1), // far top left
        XMVectorSet(1,  1, 1, 1)  // far top right
    };

    // 2) ViewProjection ����� ����� ���
    XMVECTOR det;
    XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

    // 3) NDC -> ���� ��ǥ�� ��ȯ (���������� ����)
    XMVECTOR worldCorners[8];
    for (int i = 0; i < 8; ++i)
    {
        worldCorners[i] = XMVector3TransformCoord(ndcCorners[i], invViewProj);
    }

    // 4) �� ����� �� ������ ����: XMPlaneFromPoints �̿�
    // ������ ���� ����� �ո� ���� ������ ������ �� ������ ����
    InFrustum[0] = XMPlaneFromPoints(worldCorners[0], worldCorners[2], worldCorners[6]); // Left plane (x = -1)
    InFrustum[1] = XMPlaneFromPoints(worldCorners[1], worldCorners[5], worldCorners[7]); // Right plane (x =  1)
    InFrustum[2] = XMPlaneFromPoints(worldCorners[2], worldCorners[3], worldCorners[7]); // Top plane   (y =  1)
    InFrustum[3] = XMPlaneFromPoints(worldCorners[0], worldCorners[4], worldCorners[5]); // Bottom     (y = -1)
    InFrustum[4] = XMPlaneFromPoints(worldCorners[0], worldCorners[1], worldCorners[3]); // Near      (z =  0)
    InFrustum[5] = XMPlaneFromPoints(worldCorners[4], worldCorners[7], worldCorners[5]); // Far   
    // 3. Constant buffer�� ���ε�
    
    
    InFrustumCBuffer->UpdateData(InFrustum.data());
    InFrustumCBuffer->UpdateConstBuffer();
}
