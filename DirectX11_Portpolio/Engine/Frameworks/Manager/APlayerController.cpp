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
    AddMovementToPhys(deltaTime);
    
}

void APlayerController::AddMovementInput(const Vector3& moveVal)
{
    PendingMovementInput += moveVal;
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
    desc.material = capsule->PhysMaterial.get();
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

void APlayerController::AddMovementToPhys(float deltaTime)
{
    if (PhysController)
    {
        PendingMovementInput.Normalize();
        Vector3 move = PendingMovementInput * MaxMovementSpeed * deltaTime;
        PxVec3 pxMove(move.x, move.y, move.z);

        PxControllerCollisionFlags flags = PhysController->move(pxMove, 0.01f, deltaTime, nullptr);

        // 위치를 캐릭터에 동기화
        PxExtendedVec3 newPos = PhysController->getPosition();
        if (ControlledCharacter)
        {
            ControlledCharacter->GetActorTransform()->
            SetPosition(Vector3((float)newPos.x, (float)newPos.y, (float)newPos.z));
        }

        PendingMovementInput = Vector3(0, 0, 0);
    }
}
