#include "Pch.h"

#include "Frameworks/Components/UStaticMeshComponent.h"
#include "Frameworks/LightComponents/UDirectionalLightComponent.h"
#include "Frameworks/LightComponents/USpotLightComponent.h"
#include "Frameworks/LightComponents/UHaloLightComponent.h"
#include "Frameworks/LightComponents/UPointLightComponent.h"

#include "ALightActor.h"



ALightActor::ALightActor()
{

	
	Mesh = CreateComponent<UStaticMeshComponent>(this, L"Lantern");
	SetRootComponent(Mesh.get());
	SpotLightComp = CreateComponent<USpotLightComponent>(this);
	SpotLightComp->SetUpAttachment(GetRootComponent());
	SpotLightComp->GetLightInfo()->strength = Vector3(23.0f, 13.0f, 23.0f);


	
	DirectionalLightComp = CreateComponent<UDirectionalLightComponent>(this);


	//HaloComp = CreateComponent<UHaloLightComponent>(this);
	//HaloComp->SetUpAttachment(GetRootComponent());

	//PointLightComp = CreateComponent<UPointLightComponent>(this);
	//PointLightComp->SetUpAttachment(GetRootComponent());

}

ALightActor::~ALightActor()
{
}

void ALightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);


}