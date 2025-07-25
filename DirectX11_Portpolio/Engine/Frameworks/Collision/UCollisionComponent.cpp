#include "HeaderCollection.h"
#include "UCollisionComponent.h"

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
