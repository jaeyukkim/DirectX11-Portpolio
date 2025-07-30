#include "HeaderCollection.h"
#include "APlayerController.h"

#include "Frameworks/Collision/UCapsuleComponent.h"
#include "Frameworks/Objects/ACharacter.h"

APlayerController::APlayerController()
{
}

void APlayerController::Possess(ACharacter* InCharacter)
{
    if(ControlledCharacter != nullptr)
    {
        UnPossess();
    }
    
    CheckNull(InCharacter);
    ControlledCharacter = InCharacter;
    InCharacter->Possess(this);
    InitPhysController();
}

void APlayerController::UnPossess()
{
    PhysController->release();
}

void APlayerController::Tick(float deltaTime)
{
    Actor::Tick(deltaTime);
    
    ProcessMoveAction();
    ProcessLookInput();
    ProcessJumpAction();
    AddMovementToPhys(deltaTime);
    
    
    PendingMovementInput = Vector3(0, 0, 0);
    
}

void APlayerController::AddRotationInput(Vector3 InVal)
{
    FTransform* T = ControlledCharacter->GetActorTransform();
    
    float yaw = InVal.x * RotationSpeed * Timer::Get()->GetDeltaTime();
    float pitch = InVal.y * RotationSpeed * Timer::Get()->GetDeltaTime();
    float roll = InVal.z * RotationSpeed * Timer::Get()->GetDeltaTime();
    
    T->AddRotation(yaw, pitch, roll);
}

void APlayerController::AddMovementInput(const Vector3& moveVal)
{
    PendingMovementInput += moveVal;
}

void APlayerController::Jump()
{
    CheckNull(ControlledCharacter);
    CheckNull(PhysController.get());

    if(!bIsFalling && !bJump)
    {
        VerticalVelocity = JumpSpeed;
        bIsFalling = true;
        bJump = true;
    }
    
}

void APlayerController::InitPhysController()
{
    CheckNull(ControlledCharacter);

    UCapsuleComponent* capsule = ControlledCharacter->Capsule.get();
    Vector3 pos = ControlledCharacter->GetRootComponent()->GetRelativeTransform()->GetPosition();

    PxCapsuleControllerDesc desc;
    desc.height = capsule->CapsuleHalfHeight*2;
    desc.radius = capsule->CapsuleRadius;
    desc.climbingMode = PxCapsuleClimbingMode::eEASY;
    desc.contactOffset = 0.1f;
    desc.stepOffset = 0.3f;
    desc.slopeLimit = cosf(PxPi / 4.0f);
    desc.upDirection = PxVec3(0, 1, 0);
    desc.position = PxExtendedVec3(pos.x, pos.y, pos.z);
    desc.material = FPhysX::Get()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);;
    desc.reportCallback = nullptr;


    PhysController = PxPtr<PxController>::make_ptr(FPhysX::Get()->
        GetControllerManager()->createController(desc));
}

void APlayerController::ProcessMoveAction()
{
    if(Keyboard::Get()->Press('W'))
    {
        MoveAction.Broadcast(Vector2(0.0f, 1.0f));
    }
    if(Keyboard::Get()->Press('A'))
    {
        MoveAction.Broadcast(Vector2(-1.0f, 0.0f));
    }
    if(Keyboard::Get()->Press('S'))
    {
        MoveAction.Broadcast(Vector2(0.0f, -1.0f));
    }
    if(Keyboard::Get()->Press('D'))
    {
        MoveAction.Broadcast(Vector2(1.0f, 0.0f));
    }
}

void APlayerController::ProcessJumpAction()
{
    if(Keyboard::Get()->Down(VK_SPACE))
    {
        JumpAction.Broadcast();
    }
}

void APlayerController::ProcessLookInput()
{
    Vector3 moveDelta = Mouse::Get()->GetMoveDelta();
    if(moveDelta.Length() > 0)
    {
        LookInput.Broadcast(moveDelta);
    }
}

void APlayerController::AddMovementToPhys(float deltaTime)
{
    CheckNull(PhysController.get());

    PendingMovementInput.Normalize();
    if (PendingMovementInput.Length()>0)
    {
        // 가속 시간 증가
        AccelTime += deltaTime;
        DeccelTime = 0.0f;

        // log 함수 이용한 가속 곡선
        float speed = MaxMovementSpeed * (1.0f - expf(-Damping * AccelTime));
        CurrentVelocity = PendingMovementInput * speed;
    }
    else
    {
        // log 함수 이용한 감속 곡선
        DeccelTime += deltaTime;
        AccelTime = 0.0f;

        float decay = expf(-Damping * DeccelTime);
        CurrentVelocity *= decay;

        // 너무 작으면 정지 처리
        if (CurrentVelocity.LengthSquared() < 1.0f)
            CurrentVelocity = Vector3::Zero;
    }

    
    Vector3 velocity = CurrentVelocity * deltaTime;

    
    VerticalVelocity += Gravity * deltaTime;
    velocity.y += VerticalVelocity;
    
    
    if (!bIsFalling || VerticalVelocity <= 0.0f)
    {
        VerticalVelocity = 0.0f;
        bJump = false;
    }
    
    PxVec3 pxMove(velocity.x, velocity.y, velocity.z);
    PxControllerCollisionFlags flags = PhysController->move(pxMove, 0.01f, deltaTime, nullptr);

    if(flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        bIsFalling = false;
    }
    else
    {
        bIsFalling = true;
    }
  
    
    // 위치를 캐릭터에 동기화
    PxExtendedVec3 newPos = PhysController->getPosition();
    if (ControlledCharacter)
    {
        ControlledCharacter->GetActorTransform()->
        SetPosition(Vector3((float)newPos.x, (float)newPos.y, (float)newPos.z));
    }
    Speed = CurrentVelocity.Length();
  
}
