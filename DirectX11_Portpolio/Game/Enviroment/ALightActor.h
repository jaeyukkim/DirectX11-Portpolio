#pragma once
#include "Frameworks/Objects/Actor.h"


class UPointLightComponent;
class UHaloLightComponent;
class UDirectionalLightComponent;
class USpotLightComponent;
class UStaticMeshComponent;


class ALightActor : public Actor
{
public:
	ALightActor();
	virtual ~ALightActor();
	virtual void Tick(float deltaTime) override;



	shared_ptr<USpotLightComponent> SpotLightComp;
	shared_ptr<UDirectionalLightComponent> DirectionalLightComp;
	shared_ptr<UHaloLightComponent> HaloComp;
	shared_ptr<UPointLightComponent> PointLightComp;

	shared_ptr<UStaticMeshComponent> Mesh;
};