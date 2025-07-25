#pragma once
#include "UCollisionComponent.h"

class UCapsuleComponent : public UCollisionComponent
{
public:
    UCapsuleComponent(FPhysicsOption InOption);
    virtual ~UCapsuleComponent() override = default;
    virtual void TickComponent(float deltaTime) override;
 
    
private:
    void InitCapsule();
    void InitPhysics();
    
    
private:
    //float CapsuleRadius = 34.0f;
    //float CapsuleHalfHeight = 80.0f ;
    float CapsuleRadius = 34.0f;
    float CapsuleHalfHeight = 60.0f ;
};
