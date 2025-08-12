#pragma once
#include "UCollisionComponent.h"

class USquareComponent : public UCollisionComponent
{
public:

    USquareComponent(Vector2 InSqaureSize = Vector2(300.0f, 300.0f),
        FPhysicsOption InOption = FPhysicsOption());
    virtual void InitComponent() override;
    virtual ~USquareComponent() override = default;
    virtual void TickComponent(float deltaTime) override;

public:
    void InitSqare();
    void InitPhysics();

private:
    PxPtr<PxMaterial> PhysMaterial;
    float SquareX = 300.0f;
    float SquareY = 300.0f;

};

