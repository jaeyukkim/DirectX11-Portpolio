#pragma once
#include "Actor.h"


class APlayerController;
class UCapsuleComponent;
class USceneComponent;
class USkeletalMeshComponent;

class ACharacter : public Actor
{
public:
	ACharacter();
	virtual ~ACharacter();
	virtual void Tick(float deltaTime) override;
	virtual void Possess(APlayerController* InPlayerController);
	virtual void UnPossess();
	

	
	APlayerController* GetPlayerController() {return PlayerController;}
	
	shared_ptr<USkeletalMeshComponent> Mesh;
	shared_ptr<UCapsuleComponent> Capsule;

protected:
	APlayerController* PlayerController = nullptr;
}; 