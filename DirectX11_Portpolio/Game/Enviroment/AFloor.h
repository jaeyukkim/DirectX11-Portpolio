#pragma once
#include "Frameworks/Objects/Actor.h"

class UStaticMeshComponent;
class AFloor : public Actor
{
public:
	AFloor();
	virtual ~AFloor() = default;

	virtual void Tick(float deltaTime) override;


public:
	shared_ptr<UStaticMeshComponent> floor;

};