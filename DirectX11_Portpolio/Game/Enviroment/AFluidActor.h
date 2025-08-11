#pragma once
#include "Frameworks/Objects/Actor.h"
#include "Frameworks/Particle/StableFluid.h"

class AFluidActor : public Actor
{
public:
    virtual void InitAllComponents() override;
    virtual void Tick(float deltaTime) override;


private:
    StableFluid m_StableFluid;
    StaticMeshData meshData;
    FFluidTexture FloorTex;
    FTransform TexTransform;
    Matrix TransformMat;
    
    shared_ptr<ConstantBuffer> TransformCBuffer;
    shared_ptr<IndexBuffer> IBuffer;
    shared_ptr<VertexBuffer> VBuffer;

    UINT TexWidth = 2048;
    UINT TexHeight = 2048;
};
