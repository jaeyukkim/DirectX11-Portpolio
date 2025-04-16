#pragma once
#include "Actor.h"
#include "ESkeletalMeshComponent.h"


class ESceneComponent;


class ACharacter : public Actor
{
public:
	ACharacter();
	~ACharacter();

	shared_ptr<ESkeletalMeshComponent> Mesh;
	
}; 