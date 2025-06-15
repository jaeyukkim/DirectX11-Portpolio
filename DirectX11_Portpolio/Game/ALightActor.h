#pragma once
#include "Frameworks/Objects/Actor.h"


class UStaticMeshComponent;
class ULightComponent;

class ALightActor : public Actor
{
public:
	ALightActor();
	virtual ~ALightActor();
	virtual void Tick(float deltaTime) override;


	shared_ptr<ULightComponent> LightComp;
	shared_ptr<ULightComponent> DirectionalLightComp;
	shared_ptr<UStaticMeshComponent> Mesh;
};