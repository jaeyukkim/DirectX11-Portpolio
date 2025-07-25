#include "HeaderCollection.h"
#include "UDirectionalLightComponent.h"

UDirectionalLightComponent::UDirectionalLightComponent()
    :ULightComponent(ELightType::LT_Directional | ELightType::LT_UseShadow)

{
    GetRelativeTransform()->SetPosition(Vector3(-700, 900, 500));
    Vector3 LightDirection = Vector3(0.0f, 0.0f, 0.0f) - GetRelativeTransform()->GetPosition();
    LightDirection.Normalize();
   
    GetLightInfo()->direction = LightDirection;
    GetLightInfo()->strength = Vector3(1.0f, 0.93f, 0.82f)*2.0f;
}

void UDirectionalLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);
    
    UpdateLight();
}
