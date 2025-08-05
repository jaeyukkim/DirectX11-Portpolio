#include "Pch.h"
#include "AEnemy.h"

#include "Enemy/UEnemyAnimInstance.h"

AEnemy::AEnemy()
{
    Mesh = CreateComponent<USkeletalMeshComponent>(this, L"Paladin");
    Mesh->SetUpAttachment(GetRootComponent());
    GetRootComponent()->GetRelativeTransform()->bLockPitch = true;
    GetRootComponent()->GetRelativeTransform()->bLockRoll = true;
}


void AEnemy::Tick(float deltaTime)
{
    ACharacter::Tick(deltaTime);
}

void AEnemy::InitEnemy()
{
    if(Mesh != nullptr)
    {
        Mesh->CreateAnimInstance<UEnemyAnimInstance>(this);
    }
    
}
