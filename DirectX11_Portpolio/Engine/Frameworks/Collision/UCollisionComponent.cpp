#include "HeaderCollection.h"
#include "UCollisionComponent.h"

#include "Frameworks/Objects/Actor.h"

UCollisionComponent::UCollisionComponent(const ECollisionShape& InCollisionShape)
    :CollisionShape(InCollisionShape)
{
}

UCollisionComponent::~UCollisionComponent()
{
}

void UCollisionComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);
}

void UCollisionComponent::CreatePrimitive()
{
}

void UCollisionComponent::InitComponent()
{
    Super::InitComponent();

    
    if(RigidDynamic.get() != nullptr)
    {
        RigidDynamic->userData = static_cast<void*>(GetOwner());
    }
    if(RigidStatic.get() != nullptr)
    {
        RigidStatic->userData = static_cast<void*>(GetOwner());
    }
}
