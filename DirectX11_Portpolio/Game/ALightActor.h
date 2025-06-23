#pragma once
#include "Frameworks/Objects/Actor.h"


class UDirectionalLightComponent;
class USpotLightComponent;
class UStaticMeshComponent;


class ALightActor : public Actor
{
public:
	ALightActor();
	virtual ~ALightActor();
	virtual void Tick(float deltaTime) override;


	shared_ptr<USpotLightComponent> LightComp;
	shared_ptr<UDirectionalLightComponent> DirectionalLightComp;
	shared_ptr<UStaticMeshComponent> Mesh;
};