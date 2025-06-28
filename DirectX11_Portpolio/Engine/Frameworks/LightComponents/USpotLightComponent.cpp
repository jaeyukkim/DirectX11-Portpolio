#include "HeaderCollection.h"
#include "USpotLightComponent.h"

USpotLightComponent::USpotLightComponent()
    :ULightComponent(LT_Spot | LT_UseShadow)
{
}

USpotLightComponent::~USpotLightComponent()
{
}

void USpotLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);

    
    UpdateLight();
}
