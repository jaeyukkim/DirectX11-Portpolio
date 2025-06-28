#include "HeaderCollection.h"
#include "UDirectionalLightComponent.h"

UDirectionalLightComponent::UDirectionalLightComponent()
    :ULightComponent(ELightType::LT_Directional)

{
    //GetRelativeTransform()->SetPosition(Vector3(-750, 900, 500));
    Vector3 LightDirection = Vector3(0.0f, 0.0f, 0.0f) - Vector3(-7, 9, 5);
    LightDirection.Normalize();
   
    GetLightInfo()->direction = LightDirection;
    GetLightInfo()->strength = Vector3(1.0f, 0.93f, 0.82f)*2.0f;
}

void UDirectionalLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);
    
    UpdateLight();
}
