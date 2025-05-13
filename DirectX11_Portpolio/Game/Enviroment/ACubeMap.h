#pragma once
#include "Frameworks/Actor.h"

class UStaticMeshComponent;


class ACubeMap : public Actor
{
public:
	ACubeMap();
	virtual ~ACubeMap() = default;

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;


    

private:
	shared_ptr<UStaticMeshComponent> Sphere;
};