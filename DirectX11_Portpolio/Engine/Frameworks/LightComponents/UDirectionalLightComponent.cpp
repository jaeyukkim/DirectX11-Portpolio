#include "HeaderCollection.h"
#include "UDirectionalLightComponent.h"

UDirectionalLightComponent::UDirectionalLightComponent()
    :ULightComponent(ELightType::LT_Directional | ELightType::LT_UseShadow)

{
}

void UDirectionalLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);
    UpdateLight();
}
