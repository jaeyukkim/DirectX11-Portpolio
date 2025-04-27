#pragma once
#include "Frameworks/Actor.h"

class UStaticMeshComponent;
class AFloor : public Actor
{
public:
	AFloor();
	virtual ~AFloor() = default;

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;


public:
	shared_ptr<UStaticMeshComponent> floor;

};