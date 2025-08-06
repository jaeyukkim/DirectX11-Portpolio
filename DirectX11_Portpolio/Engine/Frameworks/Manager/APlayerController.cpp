#include "HeaderCollection.h"
#include "APlayerController.h"

#include "Frameworks/Collision/UCapsuleComponent.h"
#include "Frameworks/Objects/ACharacter.h"

APlayerController::APlayerController()
{
}

void APlayerController::Possess(ACharacter* InCharacter)
{
    Super::Possess(InCharacter);
}

void APlayerController::UnPossess()
{
    Super::UnPossess();
}

void APlayerController::Tick(float deltaTime)
{
    ProcessMoveAction();
    ProcessLookInput();
    ProcessJumpAction();
    
    Super::Tick(deltaTime);
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