#pragma once

#include "Frameworks/Objects/Actor.h"

class UStaticMeshComponent;


class ACubeMap : public Actor
{
public:
	ACubeMap();
	virtual ~ACubeMap() = default;

	virtual void Tick(float deltaTime) override;



private:
	shared_ptr<UStaticMeshComponent> Sphere;
	float IBLStength = 2.5f;
};