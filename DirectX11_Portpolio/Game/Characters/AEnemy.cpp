#include "Pch.h"
#include "AEnemy.h"

#include "Enemy/UEnemyAnimInstance.h"

AEnemy::AEnemy()
{
    SetMaxHealth(200);
    SetHealth(MaxHealth);
    SetAttackDamage(10);
    SetTeamID(ETeamID::TID_BLUE);
    
    Mesh = CreateComponent<USkeletalMeshComponent>(this, L"Paladin");
    Mesh->SetUpAttachment(GetRootComponent());
    GetRootComponent()->GetRelativeTransform()->bLockPitch = true;
    GetRootComponent()->GetRelativeTransform()->bLockRoll = true;
}


void AEnemy::Tick(float deltaTime)
{
    ACharacter::Tick(deltaTime);
}


void AEnemy::Possess(AController* InController)
{
    ACharacter::Possess(InController);

    if(Mesh != nullptr)
    {
        Mesh->CreateAnimInstance<UEnemyAnimInstance>(this);
        InitMontage();
    }
    
    AIController = reinterpret_cast<AAIController*>(InController);
}

void AEnemy::TakeDamage(ICombatInterface* damageCauser, LL damageAmount)
{
    ICombatInterface::TakeDamage(damageCauser, damageAmount);
    if(bDamaged)
    {
        Mesh->GetAnimInstance()->PlayAnimMontage(HitReactMontage.get());
    }
    
}

void AEnemy::InitMontage()
{
    HitReactMontage = make_shared<AnimMontage>("sword_and_shield_impact_3");
    
}
