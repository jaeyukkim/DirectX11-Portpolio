#include "Pch.h"
#include "ICombatInterface.h"

ICombatInterface::ICombatInterface()
    :Health(0), MaxHealth(0), AttackDamage(0), TeamID(ETeamID::TID_RED), bDead(false),
    bDamaged(false)
{
}


void ICombatInterface::SendDamage(ICombatInterface* InTarget, LL damageAmount)
{
    if(IsFriend(InTarget)) return;
    
    InTarget->TakeDamage(this, damageAmount);
}

void ICombatInterface::TakeDamage(ICombatInterface* damageCauser, LL damageAmount)
{
    Health -= damageAmount;
    string log = "HP : " + std::to_string(Health);
    World::PushLog(log, Color(1, 0, 0, 1));
 

    if(Health <= 0)
        bDead = true;
    if(!bDead)
    {
        bDamaged = true;
        bCanAttack = false;
        bCanMove = false;
        
        TimerManager::Get().SetTimer(0.5f, [this]()
        {
            bDamaged = false;
            bCanAttack = true;
            bCanMove = true;
        }, false);
    }
}