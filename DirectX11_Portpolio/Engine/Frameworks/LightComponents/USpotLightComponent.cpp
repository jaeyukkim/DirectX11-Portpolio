#include "HeaderCollection.h"
#include "USpotLightComponent.h"

USpotLightComponent::USpotLightComponent()
    :ULightComponent(ELightType::LT_Spot | ELightType::LT_UseShadow)
{
}

void USpotLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);

    FTransform* transform = GetWorldTransform();
    Vector3 defaultDirection = Vector3(0.0f, -1.0f, 0.0f);
    Vector3 rotatedDirection = XMVector3Rotate(defaultDirection, transform->Rotation);
    LightInfo.direction = XMVector3Normalize(rotatedDirection);
    
    UpdateLight();
}
