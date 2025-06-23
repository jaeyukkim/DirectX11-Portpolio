#include "HeaderCollection.h"
#include "UPointLightComponent.h"

UPointLightComponent::UPointLightComponent()
    :ULightComponent(ELightType::LT_Point | LT_UseShadow)
{
}

void UPointLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);
    UpdateLight();
    
}
