#include "HeaderCollection.h"
#include "UDirectionalLightComponent.h"

#include "../../../Game/System/ULevel.h"
#include "../../../Game/System/World.h"
#include "Frameworks/Objects/ACharacter.h"

UDirectionalLightComponent::UDirectionalLightComponent()
    :ULightComponent(ELightType::LT_Directional | ELightType::LT_UseShadow)

{
    LightInfo.radius = 30.0f;
    GetRelativeTransform()->SetPosition(LightPos);
    Vector3 LightDirection = Vector3(0.0f, 0.0f, 0.0f) - LightPos;
    LightDirection.Normalize();
   
    GetLightInfo()->direction = LightDirection;
    GetLightInfo()->strength = Vector3(1.0f, 0.93f, 0.82f)*2.0f;
}

void UDirectionalLightComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);

    ACharacter* character = World::GetLevel()->GetPlayerCharacter();
    Vector3 playerPos = character->GetActorTransform()->GetPosition();
    GetRelativeTransform()->SetPosition(playerPos + LightPos);
    UpdateLight();
}
