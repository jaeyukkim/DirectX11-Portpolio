#include "Pch.h"
#include "UEnemyAnimInstance.h"
#include "Characters/AEnemy.h"

UEnemyAnimInstance::UEnemyAnimInstance(USkeletalMeshComponent* meshComp)
    :UAnimInstance(meshComp)
{
}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    UAnimInstance::NativeInitializeAnimation();
    
    if(Actor* Owner = GetActorOwner())
    {
        m_Enemy = dynamic_cast<AEnemy*>(Owner);
        Assert(m_Enemy!=nullptr, "DynamicCast Failed");
    }
    
    CreateNode();
    InitFirstNode("sword_and_shield_idle");
}

void UEnemyAnimInstance::NativeUpdateAnimation(float deltaTime)
{
    UAnimInstance::NativeUpdateAnimation(deltaTime);

    bDead = m_Enemy->IsDead();
}

void UEnemyAnimInstance::CreateNode()
{
    {
        FAnimStateNode idle;
        idle.NodeName = "sword_and_shield_idle";
        idle.TakeBlendTime = 0.15f;

        FAnimTransition idleToDead;
        idleToDead.NextNodeName = "sword_and_shield_death_2";
        idleToDead.Condition = [this](){return bDead;};

        idle.Transitions.push_back(idleToDead);
        
        AddNode(idle);
    }

    {
        FAnimStateNode dead;
        dead.NodeName = "sword_and_shield_death_2";
        dead.TakeBlendTime = 0.15f;
        dead.bLoop = false;

        AddNode(dead);
    }
}
