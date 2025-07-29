#pragma once
#include "UCollisionComponent.h"

class UBoxComponent : public UCollisionComponent
{
public:
    UBoxComponent(Vector3 InBoxSize = Vector3(300.0f, 300.0f, 300.0f),
        FPhysicsOption InOption = FPhysicsOption());
    virtual ~UBoxComponent() override = default;
    virtual void TickComponent(float deltaTime) override;

public:
    void InitBox();
    void InitPhysics();

private:
    PxPtr<PxMaterial> PhysMaterial;
    float BoxX = 300.0f;
    float BoxY = 300.0f;
    float BoxZ  = 300.0f;
};
