#pragma once
#include <Frameworks/Components/UPrimitiveComponent.h>
#include "CollisionDefinition.h"


class UCollisionComponent : public UPrimitiveComponent
{
public:
    UCollisionComponent(const ECollisionShape& InCollisionShape);
    virtual ~UCollisionComponent() override;
    virtual void TickComponent(float deltaTime) override;
    virtual void CreatePrimitive();
    
public:
    FPhysicsOption PhysicsOption;

protected:
    PxPtr<PxRigidDynamic> RigidDynamic;
    PxPtr<PxRigidStatic> RigidStatic;


protected:
    ECollisionShape CollisionShape;
};
