#include "HeaderCollection.h"
#include "USpringArmConponent.h"

USpringArmConponent::USpringArmConponent()
{
}

void USpringArmConponent::TickComponent(float deltaTime)
{
    USceneComponent::TickComponent(deltaTime);
}

void USpringArmConponent::AddLookInput(Vector2 InMoveDelta)
{
    FTransform* T = GetRelativeTransform();
    float pitch = InMoveDelta.y * RotationSpeed * Timer::Get()->GetDeltaTime();
    float yaw = InMoveDelta.x * RotationSpeed * Timer::Get()->GetDeltaTime();

    T->AddRotation(yaw, pitch, 0.0f);
}
