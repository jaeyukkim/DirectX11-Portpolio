#include "Pch.h"
#include "Light/ULightComponent.h"
#include "ALightActor.h"

#include "Frameworks/UStaticMeshComponent.h"

ALightActor::ALightActor()
{
	Mesh = CreateComponent<UStaticMeshComponent>(this, L"Lantern");
	SetRootComponent(Mesh.get());
	LightComp = CreateComponent<ULightComponent>(this, ELightType::Spot);
	LightComp->SetUpAttachment(GetRootComponent());
	//DirectionalLightComp = CreateComponent<ULightComponent>(this, ELightType::Directional);
	//DirectionalLightComp->GetLightInfo()->direction = Vector3(-1.0f, 0.0f, 0.0f);
	//DirectionalLightComp->GetLightInfo()->strength = Vector3(15.5f, 15.5f, 15.5f);
	//DirectionalLightComp->SetUpAttachment(GetRootComponent());
	LightComp->CachedEulerRotation = GetActorTransform()->GetRotation();
	LightComp->GetLightInfo()->strength = Vector3(40.5f, 40.5f, 85.5f);


}

ALightActor::~ALightActor()
{
}

void ALightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);


}

void ALightActor::Render()
{
	Super::Render();

	//DirectionalLightComp->RenderComponent();
	LightComp->RenderComponent();
	Mesh->RenderComponent();
	
}
