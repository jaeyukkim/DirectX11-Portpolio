#include "Pch.h"
#include "Light/ULightComponent.h"
#include "LightActor.h"

#include "Frameworks/UStaticMeshComponent.h"

LightActor::LightActor()
{
	LightComp = make_shared<ULightComponent>(ELightType::Lim);
	Mesh = make_shared<UStaticMeshComponent>(L"Cube");
}

LightActor::~LightActor()
{
}

void LightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	LightComp->TickComponent(deltaTime);
	Mesh->TickComponent(deltaTime);
}

void LightActor::Render()
{
	Super::Render();
	LightComp->RenderComponent();
	Mesh->RenderComponent();
}
