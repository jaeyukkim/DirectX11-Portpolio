#include "Pch.h"
#include "AKachujin.h"
#include "Frameworks/Camera/UCameraComponent.h"
#include "Frameworks/Manager/APlayerController.h"
#include "Player/UMyAnimInstance.h"
#include "Frameworks/Camera/USpringArmConponent.h"

AKachujin::AKachujin()
{
    SetMaxHealth(300);
    SetHealth(MaxHealth);
    SetAttackDamage(10);
    SetTeamID(ETeamID::TID_RED);

    shared_ptr<Converter> converter = make_shared<Converter>();
    //converter->ImportFBXFile(L"Paladin", EMeshType::SkeletalMeshType);
    
    /*
    string animName[10] =
    {
        "Melee_Attack1",
        "Melee_Attack2",
        "Melee_Attack3",
        "Melee_Attack4",
        "sword_and_shield_run",
        "sword_and_shield_idle",
        "sword_and_shield_walk",
        "sword_and_shield_jump",
        "sheath_sword_1",
        "sword_and_shield_impact_3"
    };
    
    for(string str : animName)
    {
        converter->ImportFBX_Animation(L"Paladin", String::ToWString(str));
    }
    
    string rootMotion = "sword_and_shield_death_2";
    converter->ImportFBX_Animation(L"Paladin", String::ToWString(rootMotion));
    */
    
    
    Mesh = CreateComponent<USkeletalMeshComponent>(this, L"Paladin");
    Mesh->SetUpAttachment(GetRootComponent());

    SpringArm = CreateComponent<USpringArmConponent>(this);
    SpringArm->SetUpAttachment(GetRootComponent());
    
    Camera = CreateComponent<UCameraComponent>(this);
    Camera->SetUpAttachment(SpringArm.get());
    Camera->GetRelativeTransform()->SetPosition(0.0f, 150.0f, -200.0f);
    Camera->GetRelativeTransform()->SetRotation(25.0f, 0.0f, 0.0f);

    GetRootComponent()->GetRelativeTransform()->bLockPitch = true;
    GetRootComponent()->GetRelativeTransform()->bLockRoll = true;
    
    InitMontage();
    
}

AKachujin::~AKachujin()
{
}

void AKachujin::Possess(AController* InController)
{
    Super::Possess(InController);
    
    PlayerController = reinterpret_cast<APlayerController*>(InController);
    PlayerController->MoveAction.Add(this, &AKachujin::MoveCharacter);
    PlayerController->JumpAction.Add(this, &AKachujin::JumpCharacter);
    PlayerController->LookInput.Add(this, &AKachujin::LookAction);
    
    if(Mesh != nullptr)
    {
        Mesh->CreateAnimInstance<UMyAnimInstance>(this);
    }
    
}


void AKachujin::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if(Mouse::Get()->Down(MouseButton::Left))
    {
        Attack();
    }
    if(Mouse::Get()->Down(MouseButton::Right))
    {
        AbilityRMB();
    }
}

void AKachujin::InitNotify()
{
    CanAtkDelegate.Add(this, &AKachujin::CanAttack);
    CanNotAtkDelegate.Add(this, &AKachujin::CanNotAttack);
    CanMoveDelegate.Add(this, &AKachujin::CanMove);
    CanNotMoveDelegate.Add(this, &AKachujin::CanNotMove);
    BasicAttackDelegate.Add(this, &AKachujin::BasicAttack);
    
    Notifies.emplace("CanAttack", FAnimationNotifyEvent(&CanAtkDelegate, 0.75f));
    Notifies.emplace("CanNotAttack", FAnimationNotifyEvent(&CanNotAtkDelegate, 0.01f));
    Notifies.emplace("CanMove", FAnimationNotifyEvent(&CanMoveDelegate, 0.98f));
    Notifies.emplace("CanNotMove", FAnimationNotifyEvent(&CanNotMoveDelegate, 0.01f));

    Notifies.emplace("BasicAttack0", FAnimationNotifyEvent(&BasicAttackDelegate, 0.43f));
    Notifies.emplace("BasicAttack1", FAnimationNotifyEvent(&BasicAttackDelegate, 0.45f));
    Notifies.emplace("BasicAttack2", FAnimationNotifyEvent(&BasicAttackDelegate, 0.53f));
    
    Notifies.emplace("AbilityRMBAttack1", FAnimationNotifyEvent(&BasicAttackDelegate, 0.63f));
    Notifies.emplace("AbilityRMBAttack2", FAnimationNotifyEvent(&BasicAttackDelegate, 0.97f));


}

void AKachujin::TakeDamage(ICombatInterface* damageCauser, LL damageAmount)
{
    ICombatInterface::TakeDamage(damageCauser, damageAmount);
    if(bDamaged)
    {
        Mesh->GetAnimInstance()->PlayAnimMontage(HitReactMontage.get());
    }
}

void AKachujin::InitMontage()
{
    InitNotify();
    
    AttackMontage.emplace_back(AnimMontage("Melee_Attack1"));
    AttackMontage.emplace_back(AnimMontage("Melee_Attack2"));
    AttackMontage.emplace_back(AnimMontage("Melee_Attack3"));
    MaxAttackCount = AttackMontage.size();
    for(AnimMontage& montage : AttackMontage)
    {
        montage.PlaySpeed = 1.5f;
        montage.Triggers.push_back(&Notifies["CanAttack"]);
        montage.Triggers.push_back(&Notifies["CanNotAttack"]);
        montage.Triggers.push_back(&Notifies["CanMove"]);
        montage.Triggers.push_back(&Notifies["CanNotMove"]);
    }
    AttackMontage[0].Triggers.push_back(&Notifies["BasicAttack0"]);
    AttackMontage[1].Triggers.push_back(&Notifies["BasicAttack1"]);
    AttackMontage[2].Triggers.push_back(&Notifies["BasicAttack2"]);

    
    AbilityRMBMontage = make_shared<AnimMontage>("Melee_Attack4");
    AbilityRMBMontage->PlaySpeed = 1.7f;
    AbilityRMBMontage->Triggers.push_back(&Notifies["CanAttack"]);
    AbilityRMBMontage->Triggers.push_back(&Notifies["CanMove"]);
    AbilityRMBMontage->Triggers.push_back(&Notifies["CanNotMove"]);
    AbilityRMBMontage->Triggers.push_back(&Notifies["AbilityRMBAttack1"]);
    AbilityRMBMontage->Triggers.push_back(&Notifies["AbilityRMBAttack2"]);
    
    HitReactMontage = make_shared<AnimMontage>("sword_and_shield_impact_3");
   
}



void AKachujin::LookAction(Vector3 InValue)
{
    //Camera->AddLookInput(Vector2(InValue.x, InValue.y));
    
    //PlayerController->AddRotationInput(InValue);
    SpringArm->AddLookInput(Vector2(InValue.x, InValue.y));
    
}

void AKachujin::MoveCharacter(Vector2 InValue)
{
    CheckNull(PlayerController);
    CheckFalse(bCanMove);
	
    Vector3 Forward = Camera->GetWorldTransform()->GetForwardVector();
    Vector3 Right   = Camera->GetWorldTransform()->GetRightVector();
    Vector3 MoveDir = (Forward * InValue.y) + (Right * InValue.x);
    MoveDir.y = 0;


    if (MoveDir.LengthSquared() > 0.0f)
    {
        MoveDir.Normalize();
        PlayerController->AddMovementInput(MoveDir);

     
        FTransform* ActorTransform = GetActorTransform();
        Quaternion currentRot = ActorTransform->GetQuat();
        
        Vector3 forward = MoveDir;
        Vector3 up = Vector3::Up;
        Vector3 right = up.Cross(forward);
        right.Normalize();
        up = forward.Cross(right);
        
        Matrix lookMatrix = Matrix
        (
            right.x,    right.y,    right.z,    0.0f,
            up.x,       up.y,       up.z,       0.0f,
            forward.x,  forward.y,  forward.z,  0.0f,
            0.0f,       0.0f,       0.0f,       1.0f
        );

        // 목표 회전 쿼터니언
        Quaternion targetRot = Quaternion::CreateFromRotationMatrix(lookMatrix);
        float t = std::clamp(RotateSpeed * Timer::Get()->GetDeltaTime(), 0.0f, 1.0f);
        Quaternion smoothedRot = Quaternion::Slerp(currentRot, targetRot, t);
        smoothedRot.Normalize();
        ActorTransform->SetQuat(smoothedRot);
    }
    
	
}

void AKachujin::JumpCharacter()
{
    if(PlayerController == nullptr) return;

    PlayerController->Jump();
}

void AKachujin::Attack()
{
    CheckFalse(bCanAttack);
    
    AttackCount = AttackCount % MaxAttackCount;
    Mesh->GetAnimInstance()->PlayAnimMontage(&AttackMontage[AttackCount]);
    AttackCount++;
}

void AKachujin::AbilityRMB()
{
    Mesh->GetAnimInstance()->PlayAnimMontage(AbilityRMBMontage.get());
}

void AKachujin::BasicAttack()
{
    vector<FHitResult> hitResults;
    Vector3 pos = GetActorTransform()->GetPosition();
    Vector3 attackPos = pos + GetActorTransform()->GetForwardVector() * 110.0f;
    World::SphereTraceMulti(hitResults, attackPos, 50.0f);

    for(FHitResult& result : hitResults)
    {
        if(result.HitActor == nullptr) continue;
        ICombatInterface* target = dynamic_cast<ICombatInterface*>(result.HitActor);
        if(target == nullptr) continue;

        LL damage = AttackDamage + (AttackDamage * (AttackCount * 0.2));
        SendDamage(target, damage);
    }
}

void AKachujin::CanAttack()
{
    bCanAttack = true;
}

void AKachujin::CanNotAttack()
{
    bCanAttack = false;
}

void AKachujin::CanMove()
{
    bCanMove = true;
}

void AKachujin::CanNotMove()
{
    bCanMove = false;
}
