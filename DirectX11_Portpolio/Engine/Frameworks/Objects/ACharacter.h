#pragma once
#include "Actor.h"


class AController;
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
	virtual void Possess(AController* InController);
	virtual void UnPossess();
	

	
	AController* GetController() {return Controller;}
	
	shared_ptr<USkeletalMeshComponent> Mesh;
	shared_ptr<UCapsuleComponent> Capsule;

protected:
	AController* Controller = nullptr;
}; 