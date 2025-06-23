#include "HeaderCollection.h"
#include "ULightComponent.h"

/**
 * 
 * @param InLightType ELightType의 형식으로 넣을것
 */
ULightComponent::ULightComponent(UINT InLightType)
{
    LightInfo.LightType = InLightType;
    FSceneView::Get()->AddToLightMap(&LightInfo);
}

ULightComponent::~ULightComponent()
{
    FSceneView::Get()->DeleteFromLightMap(LightInfo.LightID);
}

void ULightComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);
    
    FTransform* transform = GetWorldTransform();
    LightInfo.position = transform->GetPosition();
}

void ULightComponent::UpdateLight()
{
    FSceneView::Get()->UpdateLightMap(&LightInfo);
}
