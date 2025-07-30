#pragma once
#include "Frameworks/Components/USceneComponent.h"

class USpringArmConponent : public USceneComponent
{
public:
    
    USpringArmConponent();
    virtual ~USpringArmConponent() = default;
    virtual void TickComponent(float deltaTime) override;
    void AddLookInput(Vector2 InMoveDelta);

public:
    float RotationSpeed = 25.f;
    float TargetArmLength = 300.0f;
    float bUsePawnControlRotation = true;
};
