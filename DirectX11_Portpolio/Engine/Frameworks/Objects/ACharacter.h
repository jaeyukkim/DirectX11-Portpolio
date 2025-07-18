#pragma once
#include "Actor.h"



class USceneComponent;
class USkeletalMeshComponent;

class ACharacter : public Actor
{
public:
	ACharacter();
	virtual ~ACharacter();
	virtual void Tick(float deltaTime) override;


	vector<shared_ptr<USkeletalMeshComponent>> Mesh;

}; 