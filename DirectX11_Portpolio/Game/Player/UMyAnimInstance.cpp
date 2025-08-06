#include "Pch.h"
#include "UMyAnimInstance.h"

#include "../Characters/AKachujin.h"
#include "Frameworks/Manager/APlayerController.h"

UMyAnimInstance::UMyAnimInstance(USkeletalMeshComponent* meshComp)
    :UAnimInstance(meshComp)
{
}

void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    if(Actor* Owner = GetActorOwner())
    {
        Paladin = dynamic_cast<AKachujin*>(Owner);
        Assert(Paladin!=nullptr, "DynamicCast Failed");
    }
    
    Controller = Paladin->GetPlayerController();
    Assert(Controller!=nullptr, "Controller NullPtr");
    
    CreateNode();
    InitFirstNode("sword_and_shield_idle");
}

void UMyAnimInstance::NativeUpdateAnimation(float deltaTime)
{
    Super::NativeUpdateAnimation(deltaTime);

    bFalling = Controller->IsFalling();
    Speed = Controller->GetSpeed();
    Direction = Controller->GetVelocity();
    bDead = Paladin->IsDead();
    Direction.Normalize();


}


void UMyAnimInstance::CreateNode()
{
    {
        FAnimStateNode idle;
        idle.NodeName = "sword_and_shield_idle";
        idle.TakeBlendTime = 0.15f;
        
        FAnimTransition idleToWalk;
        idleToWalk.NextNodeName = "sword_and_shield_walk";
        idleToWalk.Condition = [this](){return Speed > 20 && !bFalling && !bDead;};

        FAnimTransition idleToJump;
        idleToJump.NextNodeName = "sword_and_shield_jump";
        idleToJump.Condition = [this](){return bFalling && !bDead;};

        FAnimTransition idleToDead;
        idleToDead.NextNodeName = "sword_and_shield_death_2";
        idleToDead.Condition = [this](){return bDead;};
        
        idle.Transitions.push_back(idleToWalk);
        idle.Transitions.push_back(idleToJump);
        idle.Transitions.push_back(idleToDead);
        AddNode(idle);
    }

    {
        FAnimStateNode jump;
        jump.NodeName = "sword_and_shield_jump";
        jump.TakeBlendTime = 0.1f;
        jump.bLoop = false;
        
        FAnimTransition transJump;
        transJump.NextNodeName = "sword_and_shield_idle";
        transJump.Condition = [this](){return !bFalling;};
        
        jump.Transitions.push_back(transJump);
        AddNode(jump);
    }

    {
        FAnimStateNode walkFoward;
        walkFoward.NodeName = "sword_and_shield_walk";
        walkFoward.TakeBlendTime = 0.1f;
        
        FAnimTransition walkToIdle;
        walkToIdle.NextNodeName = "sword_and_shield_idle";
        walkToIdle.Condition = [this](){return Speed < 20 && !bFalling;};

        FAnimTransition walkToRun;
        walkToRun.NextNodeName = "sword_and_shield_run";
        walkToRun.Condition = [this](){return Speed > 400 && !bFalling;};
        
        walkFoward.Transitions.push_back(walkToIdle);
        walkFoward.Transitions.push_back(walkToRun);
        AddNode(walkFoward);
    }

    {
        FAnimStateNode walkFoward;
        walkFoward.NodeName = "sword_and_shield_run";
        walkFoward.TakeBlendTime = 1.0f;
        
        FAnimTransition RunToIdle;
        RunToIdle.NextNodeName = "sword_and_shield_idle";
        RunToIdle.Condition = [this](){return Speed < 20 && !bFalling;};
        walkFoward.Transitions.push_back(RunToIdle);
        
        AddNode(walkFoward);
    }

    {
        FAnimStateNode dead;
        dead.NodeName = "sword_and_shield_death_2";
        dead.TakeBlendTime = 0.15f;
        dead.bLoop = false;

        AddNode(dead);
    }
}

