#include "HeaderCollection.h"
#include "UHaloLightComponent.h"

UHaloLightComponent::UHaloLightComponent()
    :ULightComponent(LT_Halo | LT_UseShadow)
{
    LightInfo.radius = 3.0f;
    LightInfo.strength = Vector3(1.0f, 1.0f, 1.0f);
}

UHaloLightComponent::~UHaloLightComponent()
{
}

void UHaloLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);

    UpdateLight();

}
