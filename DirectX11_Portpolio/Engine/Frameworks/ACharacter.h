#pragma once
#include "Actor.h"
#include "USkeletalMeshComponent.h"


class USceneComponent;


class ACharacter : public Actor
{
public:
	ACharacter();
	virtual ~ACharacter();
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

	shared_ptr<USkeletalMeshComponent> Mesh;
	
}; 