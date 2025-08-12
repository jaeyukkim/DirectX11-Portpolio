#include "Pch.h"
#include "AFluidActor.h"

#include "Frameworks/Collision/UBoxComponent.h"
#include "Frameworks/Manager/AController.h"
#include "Frameworks/Objects/ACharacter.h"
#include "Render/FSceneRender.h"
#include "Render/Mesh/GeometryGenerator.h"

AFluidActor::AFluidActor()
{
    boxCollision = CreateComponent<UBoxComponent>(this, Vector3(2048.0f, 20.0f, 2048.0f));
    SetRootComponent(boxCollision.get());
    m_StableFluid.Initialize(TexWidth, TexHeight);

    meshData = GeometryGenerator::MakeSquare(1024);
    IBuffer = make_shared<IndexBuffer>(meshData.Indices.data(), meshData.Indices.size());
    VBuffer = make_shared<VertexBuffer>(meshData.Vertices.data(), meshData.Vertices.size(), sizeof(VertexObject));

    TextureBuffer::CreateSRVUAVTexture(TexWidth, TexHeight, DXGI_FORMAT_R16G16B16A16_FLOAT,
           FloorTex.texture, FloorTex.srv, FloorTex.uav);
    
    meshTransform.SetPosition(-6000.0f, -100.0f, 0.0f);
    meshTransform.SetRotation(90.0f, 0.0f, 0.0f);
    FSceneRender::Get()->AddCustomRenderObject(this);
}

void AFluidActor::InitAllComponents()
{
    Super::InitAllComponents();
   
    WorldCBuffer = make_shared<ConstantBuffer>(&meshWorld, sizeof(Matrix));
    
}

void AFluidActor::Tick(float deltaTime)
{
    static float s_prevNdcX = 0.0f, s_prevNdcY = 0.0f;
    static bool  s_injecting = false;
    static int   s_colorIdx  = 0;

    ACharacter* character = World::GetLevel()->GetPlayerCharacter();
    if (!character || !boxCollision) return;

    Vector3 playerPos = character->GetActorTransform()->GetPosition();
    Vector3 velocity  = character->GetController()->GetVelocity() * deltaTime;

    // ── 바닥, 씬, 지오메트리(half extents) ─────────────────────────────
    PxRigidStatic* floor = boxCollision->GetRigidStatic();
    if (!floor) return;

    PxShape* shape = nullptr;
    floor->getShapes(&shape, 1);
    if (!shape) { m_StableFluid.FluidConstCPU.i = -1; return; }

    PxGeometryHolder gh = shape->getGeometry();
    if (gh.getType() != PxGeometryType::eBOX) { m_StableFluid.FluidConstCPU.i = -1; return; }
    const PxBoxGeometry& boxGeo = gh.box();
    // 로컬 XZ가 바닥이라고 가정(윗면). 평면 half 크기
    Vector2 planeSize { boxGeo.halfExtents.x, boxGeo.halfExtents.z };

    // ── 레이 시작점: 바닥 법선 방향으로 살짝 올려서 아래로 쏘기 ───────
    const PxTransform pose = floor->getGlobalPose();
    const PxVec3 nWS = pose.q.rotate(PxVec3(0, 1, 0)); // 바닥 법선(+Y)
    Vector3 rayOrigin = playerPos + Vector3(nWS.x, nWS.y, nWS.z) * 10.0f; // 10cm 띄우기

    // ── 레이캐스트 + 텍스처 좌표 변환 ──────────────────────────────────
    const int texW = 1024, texH = 1024;
    float u, v, ndcX, ndcY;
    int   i, j;

    bool hit = RaycastToFloorAndTexCoords(
        floor,              // PxRigidActor* (바닥)
        rayOrigin,          // 레이 시작점
        400.0f, // 레이 길이
        planeSize,          // 평면 half extents (월드 단위)
        texW, texH,
        u, v, i, j, ndcX, ndcY,
        true
    );

    if (hit)
    {
        // 주입 픽셀 좌표
        m_StableFluid.FluidConstCPU.i = i;
        m_StableFluid.FluidConstCPU.j = j;
        
        Vector2 ndcVelFrame = Vector2(ndcX - s_prevNdcX, -(ndcY - s_prevNdcY));
        m_StableFluid.FluidConstCPU.sourcingVelocity = ndcVelFrame * deltaTime;

        s_prevNdcX = ndcX;
        s_prevNdcY = ndcY;

        // 처음 주입 시작했을 때 색 한 번 바꾸기
        if (!s_injecting)
        {
            static const Vector4 kRainbow[7] = {
                {1,0,0,1},{1,0.65f,0,1},{1,1,0,1},{0,1,0,1},
                {0,0,1,1},{0.3f,0,0.5f,1},{0.5f,0,1,1}
            };
            m_StableFluid.FluidConstCPU.sourcingDensity = kRainbow[(s_colorIdx++) % 7];
            s_injecting = true;
        }
    }
    else
    {
        // 바닥을 맞지 않으면 주입 끄기
        m_StableFluid.FluidConstCPU.i = -1;
        s_injecting = false;
    }
    
}

void AFluidActor::CustomRender(float deltaTime)
{
    Super::CustomRender(deltaTime);
    m_StableFluid.Render(deltaTime);
    D3D::Get()->GetDeviceContext()->CopyResource(FloorTex.texture.Get(), m_StableFluid.DensityTex.texture.Get());

    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SimpleTexturePSO);
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 1, FloorTex.srv.GetAddressOf());

    VBuffer->IASetVertexBuffer();
    IBuffer->IASetIndexBuffer();

    meshWorld = meshTransform.ToMatrix().Transpose();
    WorldCBuffer->UpdateConstBuffer();
    WorldCBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_World, 1);
    D3D::Get()->GetDeviceContext()->DrawIndexed(meshData.Indices.size(), 0, 0);

}

void AFluidActor::FluidTestForMouse()
{
    static int color = 0;
    static bool m_prevLeftButton = false;
    static float m_prevMouseNdcX = -1.0f;
    static float m_prevMouseNdcY = -1.0f;

    Vector2 ndc = Mouse::Get()->GetMouseNDC();

    
    if (Mouse::Get()->Press(MouseButton::Left))
    {
        m_StableFluid.FluidConstCPU.i = Mouse::Get()->GetPosition().x;
        m_StableFluid.FluidConstCPU.j = Mouse::Get()->GetPosition().y;

        if (!m_prevLeftButton)
        { 

            // 랜덤하게 색 교체
            static const std::vector<Vector4> rainbow =
            {
                {1.0f, 0.0f, 0.0f, 1.0f},  // Red
                {1.0f, 0.65f, 0.0f, 1.0f}, // Orange
                {1.0f, 1.0f, 0.0f, 1.0f},  // Yellow
                {0.0f, 1.0f, 0.0f, 1.0f},  // Green
                {0.0f, 0.0f, 1.0f, 1.0f},  // Blue
                {0.3f, 0.0f, 0.5f, 1.0f},  // Indigo
                {0.5f, 0.0f, 1.0f, 1.0f}   // Violet/Purple
            };

            m_StableFluid.FluidConstCPU.sourcingDensity = rainbow[(color++) % 7];
            m_StableFluid.FluidConstCPU.sourcingVelocity = Vector2(0.0f);

        }
        else
        {
            Vector2 ndcVel = Vector2(ndc.x, -ndc.y) -
                             Vector2(m_prevMouseNdcX, -m_prevMouseNdcY);
            m_StableFluid.FluidConstCPU.sourcingVelocity = ndcVel * 10.0f;
        }
    }
    else
    {
        m_StableFluid.FluidConstCPU.i = -1; // uint의 Overflow 이용
    }

    m_prevLeftButton = Mouse::Get()->Press(MouseButton::Left);
    m_prevMouseNdcX = ndc.x;
    m_prevMouseNdcY = ndc.y;

}

bool AFluidActor::RaycastToFloorAndTexCoords(const PxRigidActor* floor, const Vector3& originWS,
                                             float maxDist, const Vector2& planeSize, int texW, int texH, float& outU, float& outV, int& outI, int& outJ,
                                             float& outNdcX, float& outNdcY, bool wrapUV)
{
    PxScene* scene = FPhysX::Get()->GetPhysScene();
    if (!scene || !floor) return false;

    const PxTransform pose = floor->getGlobalPose();
    const PxVec3 dir = PxVec3(0,-1,0); // 바닥 쪽
    
    PxRaycastBuffer hit;
    const PxQueryFilterData fd(PxQueryFlag::eSTATIC); // static만 맞추고 싶다면

    bool bHit = scene->raycast(
        PxVec3(originWS.x, originWS.y, originWS.z),
        dir, maxDist, hit,
        PxHitFlag::eDEFAULT, fd);

    if (!bHit || hit.getNbAnyHits()==0) return false;
    
    const PxRaycastHit& h = hit.getAnyHit(0);
    if (h.actor != floor) return false; // 다른 물체를 맞았으면 패스

    // 히트 포인트를 로컬로
    PxVec3 pL = pose.q.getConjugate().rotate(h.position - pose.p);

    // 로컬 XZ -> UV 정규화 (A와 동일)
    float u = (pL.x / (planeSize.x * 2.0f)) + 0.5f;
    float v = (pL.z / (planeSize.y * 2.0f)) + 0.5f;

    if (wrapUV) { u -= std::floor(u); v -= std::floor(v); }
    else        { u = std::clamp(u,0.0f,1.0f); v = std::clamp(v,0.0f,1.0f); }

    outU=u; outV=v;
    outI = std::clamp(int(std::floor(u * texW)), 0, texW - 1);
    outJ = std::clamp(int(std::floor((1.0f - v) * texH)), 0, texH - 1);
    outNdcX = u * 2.0f - 1.0f;
    outNdcY = -(v * 2.0f - 1.0f);
    return true;
    
}
