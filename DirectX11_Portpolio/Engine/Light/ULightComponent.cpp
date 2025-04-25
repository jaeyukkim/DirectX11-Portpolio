#include "HeaderCollection.h"
#include "Render/FSceneView.h"
#include "ULightComponent.h"

ULightComponent::ULightComponent(ELightType InLightType)
{
    LightInfo.LightType = InLightType;
    FSceneView::Get()->AddToLightMap(&LightInfo);
}

ULightComponent::~ULightComponent()
{
}

void ULightComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);


    FTransform* transform = GetWorldTransform();
    LightInfo.direction = Vector3(transform->GetRotation());
    LightInfo.position = transform->GetPosition();


    FSceneView::Get()->UpdateLightMap(LightInfo);
}

void ULightComponent::RenderComponent()
{
	Super::RenderComponent();
}

 
