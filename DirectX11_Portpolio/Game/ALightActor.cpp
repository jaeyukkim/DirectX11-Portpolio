#include "Pch.h"

#include "Frameworks/Components/UStaticMeshComponent.h"
#include "Frameworks/LightComponents/UDirectionalLightComponent.h"
#include "Frameworks/LightComponents/USpotLightComponent.h"
#include "ALightActor.h"


ALightActor::ALightActor()
{
	
	Mesh = CreateComponent<UStaticMeshComponent>(this, L"Lantern");
	SetRootComponent(Mesh.get());
	LightComp = CreateComponent<USpotLightComponent>(this);
	LightComp->SetUpAttachment(GetRootComponent());
	LightComp->GetLightInfo()->strength = Vector3(40.5f, 40.5f, 85.5f);

	
	DirectionalLightComp = CreateComponent<UDirectionalLightComponent>(this);
	//DirectionalLightComp->GetRelativeTransform()->SetPosition(Vector3(-75, 90, 50));
	//DirectionalLightComp->GetRelativeTransform()->SetRotation(Vector3(-33, 140, 0));
	//DirectionalLightComp->GetLightInfo()->strength = Vector3(15.5f, 15.5f, 15.5f);
	DirectionalLightComp->GetRelativeTransform()->SetPosition(Vector3(-75, 90, 50));
	Vector3 LightDirection = Vector3(0.0f, 0.0f, 0.0f) - Vector3(-75, 90, 50);
	LightDirection.Normalize();
	DirectionalLightComp->GetLightInfo()->direction = LightDirection;
}

ALightActor::~ALightActor()
{
}

void ALightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);


}