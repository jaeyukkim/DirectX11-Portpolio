#include "Pch.h"
#include "Light/ULightComponent.h"
#include "LightActor.h"

LightActor::LightActor()
{
	LightComp = make_shared<ULightComponent>(ELightType::Directional);
}

LightActor::~LightActor()
{
}

void LightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	LightComp->TickComponent(deltaTime);
}

void LightActor::Render()
{
	Super::Render();
	LightComp->RenderComponent();
}
