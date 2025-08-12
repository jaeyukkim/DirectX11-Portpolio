#include "HeaderCollection.h"
#include "USquareComponent.h"

USquareComponent::USquareComponent(Vector2 InSqaureSize, FPhysicsOption InOption)
    :UCollisionComponent(ECollisionShape::ECS_Sqare)
{
}

void USquareComponent::InitComponent()
{
    UCollisionComponent::InitComponent();
}

void USquareComponent::TickComponent(float deltaTime)
{
    UCollisionComponent::TickComponent(deltaTime);
}

void USquareComponent::InitSqare()
{
}

void USquareComponent::InitPhysics()
{
}
