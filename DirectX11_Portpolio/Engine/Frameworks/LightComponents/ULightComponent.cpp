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
    //LightInfo.direction = Vector3(transform->GetRotation());
    LightInfo.position = transform->GetPosition();

    
    Vector3 defaultDirection = Vector3(0.0f, -1.0f, 0.0f);
    Vector3 rotatedDirection = XMVector3Rotate(defaultDirection, transform->Rotation);
    LightInfo.direction = XMVector3Normalize(rotatedDirection);
    FSceneView::Get()->UpdateLightMap(&LightInfo);
    
}