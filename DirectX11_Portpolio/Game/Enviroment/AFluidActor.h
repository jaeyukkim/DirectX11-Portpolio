#pragma once
#include "Frameworks/Objects/Actor.h"
#include "Frameworks/Particle/StableFluid.h"

class UBoxComponent;



class AFluidActor : public Actor
{
public:
    AFluidActor();
    virtual void InitAllComponents() override;
    virtual void Tick(float deltaTime) override;
    virtual void CustomRender(float deltaTime) override;

    void FluidTestForMouse();
    bool RaycastToFloorAndTexCoords(
    const PxRigidActor* floor,    // 맞추길 원하는 바닥
    const Vector3& originWS,      // 레이 시작점(보통 플레이어 위치 위쪽)
    float maxDist,
    const Vector2& planeSize,
    int texW, int texH,
    float& outU, float& outV,
    int& outI, int& outJ,
    float& outNdcX, float& outNdcY,
    bool wrapUV = true);
    
private:
    shared_ptr<UBoxComponent> boxCollision;
    StableFluid m_StableFluid;
    StaticMeshData meshData;
    FFluidTexture FloorTex;
    shared_ptr<IndexBuffer> IBuffer;
    shared_ptr<VertexBuffer> VBuffer;
    
    Matrix meshWorld;
    FTransform meshTransform;
    shared_ptr<ConstantBuffer> WorldCBuffer;

    UINT TexWidth = 1024;
    UINT TexHeight = 1024;
};
