#pragma once
#include "../Combat/ICombatInterface.h"
#include "Frameworks/Objects/ACharacter.h"

class AAIController;

class AEnemy : public ACharacter, public ICombatInterface
{
public:
    AEnemy();
    virtual ~AEnemy() = default;
    virtual void Tick(float deltaTime) override;
    virtual void Possess(AController* InController) override;
    virtual void TakeDamage(ICombatInterface* damageCauser, LL damageAmount) override;
    void InitMontage();
    
private:
    AAIController* AIController;
    shared_ptr<AnimMontage> HitReactMontage;
};
