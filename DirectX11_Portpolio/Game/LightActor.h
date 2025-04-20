#pragma once
#include "Frameworks/Actor.h"


class ULightComponent;

class LightActor : public Actor
{
public:
	LightActor();
	virtual ~LightActor();
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

	shared_ptr<ULightComponent> LightComp;
};