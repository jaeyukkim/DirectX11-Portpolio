#include "Pch.h"
#include "AKachujin.h"
#include "Frameworks/Camera/UCameraComponent.h"
#include "Frameworks/Manager/APlayerController.h"
#include "Player/UMyAnimInstance.h"
#include "Frameworks/Camera/USpringArmConponent.h"

AKachujin::AKachujin()
{
    
    //shared_ptr<Converter> converter = make_shared<Converter>();
    //converter->ImportFBXFile(L"Paladin", EMeshType::SkeletalMeshType);
    
    /*
    string animName[1] =
    {
        "sword_and_shield_run"
        //"sword_and_shield_idle",
        //"sword_and_shield_walk",
        //"sword_and_shield_jump",
        //"sheath_sword_1"
    };
    for(string str : animName)
    {
        converter->ImportFBX_Animation(L"Paladin", String::ToWString(str));
    }
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

    
}

AKachujin::~AKachujin()
{
}

void AKachujin::Possess(APlayerController* InPlayerController)
{
    Super::Possess(InPlayerController);

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
    
}

void AKachujin::LookAction(Vector3 InValue)
{
    //Camera->AddLookInput(Vector2(InValue.x, InValue.y));
    
    PlayerController->AddRotationInput(InValue);
    SpringArm->AddLookInput(Vector2(0, InValue.y));
    
}

void AKachujin::MoveCharacter(Vector2 InValue)
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

void AKachujin::JumpCharacter()
{
    if(PlayerController == nullptr) return;

    PlayerController->Jump();
}