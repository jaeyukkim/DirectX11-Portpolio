#include "HeaderCollection.h"
#include "ACharacter.h"

#include "Frameworks/Collision/UCapsuleComponent.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"


ACharacter::ACharacter()
{
	FPhysicsOption option;
	option.PhysicsType = EPhysicsType::PT_Kinematic;
	option.bEnableGravity = true;
	option.bLockRotationX = true;
	option.bLockPositionZ = true;
	Capsule = make_shared<UCapsuleComponent>(option);
	SetRootComponent(Capsule.get());
}

ACharacter::~ACharacter()
{
}

void ACharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}
