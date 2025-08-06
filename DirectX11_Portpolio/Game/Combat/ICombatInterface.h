#pragma once

enum class ETeamID : uint8
{
    TID_RED,
    TID_BLUE
};

class ICombatInterface
{
public:
    ICombatInterface();

public:
    void SetMaxHealth(LL InMaxHealth) {MaxHealth = InMaxHealth;}
    void SetHealth(LL InHealth) {Health = InHealth;}
    void SetAttackDamage(int InAttackDamage) {AttackDamage = InAttackDamage;}
    void SetTeamID(ETeamID InTeamID) {TeamID = InTeamID;}
    
    ETeamID GetTeamID() const {return TeamID;}
    bool IsDead() {return bDead;}
public:
    virtual void SendDamage(ICombatInterface* InTarget, LL damageAmount);
    virtual void TakeDamage(ICombatInterface* damageCauser, LL damageAmount);
    bool IsFriend(const ICombatInterface* InTarget) const {return InTarget->GetTeamID() == TeamID;}
    
protected:
    LL Health;
    LL MaxHealth;
    int AttackDamage;
    ETeamID TeamID;
    bool bDead;
    bool bDamaged;
    uint8 MaxAttackCount = 0;
    uint8 AttackCount = 0;
    bool bCanAttack = true;
    bool bCanMove = true;
    
};
