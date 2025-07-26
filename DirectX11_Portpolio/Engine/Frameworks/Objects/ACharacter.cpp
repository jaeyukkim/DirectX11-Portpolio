#include "HeaderCollection.h"
#include "ACharacter.h"

#include "Frameworks/Collision/UCapsuleComponent.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"
#include "Frameworks/Manager/APlayerController.h"


ACharacter::ACharacter()
{
	FPhysicsOption option;
		option.PhysicsType = EPhysicsType::PT_Kinematic;
	
	option.bUseController = true;
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

void ACharacter::Possess(APlayerController* InPlayerController)
{
	CheckNull(PlayerController);
	PlayerController = InPlayerController;
	PlayerController->MoveAction.Add(this, &ACharacter::MoveCharacter);
}

void ACharacter::UnPossess()
{
}

void ACharacter::MoveCharacter(Vector2 InValue)
{
	CheckNull(PlayerController);

	
	Vector3 Forward = GetActorTransform()->GetForwardVector();
	Vector3 Right   = GetActorTransform()->GetRightVector();

	Vector3 MoveDir = (Forward * InValue.y) + (Right * InValue.x);

	// 방향값이 0인 경우는 무시
	if (MoveDir.LengthSquared() > 0.0f)
	{
		MoveDir.Normalize();
		PlayerController->AddMovementInput(MoveDir);
	}
	
}
