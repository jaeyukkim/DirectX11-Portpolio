#pragma once
#include "Actor.h"


class UCapsuleComponent;
class USceneComponent;
class USkeletalMeshComponent;

class ACharacter : public Actor
{
public:
	ACharacter();
	virtual ~ACharacter();
	virtual void Tick(float deltaTime) override;


	vector<shared_ptr<USkeletalMeshComponent>> Mesh;
	shared_ptr<UCapsuleComponent> Capsule;
}; 