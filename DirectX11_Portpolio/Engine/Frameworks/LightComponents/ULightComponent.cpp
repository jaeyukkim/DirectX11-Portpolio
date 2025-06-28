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
    
    if (!(LightInfo.LightType & LT_Directional))
    {
        Vector3 defaultDirection = Vector3(0.0f, 0.0f, 1.0f);
        Vector3 rotatedDirection = XMVector3Rotate(defaultDirection, transform->Rotation);
        LightInfo.direction = XMVector3Normalize(rotatedDirection);
    }
    

}

void ULightComponent::UpdateLight()
{
    

    FSceneView::Get()->UpdateLightMap(&LightInfo);
}
