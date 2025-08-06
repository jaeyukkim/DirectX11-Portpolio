#pragma once
#include "AController.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FMoveActionSignatrue, Vector2);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLookInputSignatrue, Vector3);
DECLARE_DYNAMIC_DELEGATE(FJumpActionSignatrue);


class ACharacter;



class APlayerController : public AController
{
public:
    APlayerController();
    virtual void Possess(ACharacter* InCharacter) override;
    virtual void UnPossess() override;
    virtual void Tick(float deltaTime) override;
    
public:
    void ProcessMoveAction();
    void ProcessJumpAction();
    void ProcessLookInput();

public:
    FMoveActionSignatrue MoveAction;
    FJumpActionSignatrue JumpAction;
    FLookInputSignatrue LookInput;



};
