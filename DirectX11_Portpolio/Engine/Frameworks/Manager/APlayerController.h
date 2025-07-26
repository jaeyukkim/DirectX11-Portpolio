#pragma once
#include "Frameworks/Objects/Actor.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FMoveActionSignatrue, Vector2);
DECLARE_DYNAMIC_DELEGATE_OneParam(FJumpActionSignatrue, bool);


class ACharacter;



class APlayerController : public Actor
{
public:
    APlayerController();
    virtual void Possess(ACharacter* InCharacter);
    virtual void UnPossess();
    virtual void Tick(float deltaTime) override;
    virtual void AddMovementInput(const Vector3& moveVal);
    void InitPhysController();

    
    inline ACharacter* GetConrolledCharacter() {return ControlledCharacter;}
    void ProcessMoveAction();
    void AddMovementToPhys(float deltaTime);
    
public:
    FMoveActionSignatrue MoveAction;
    FJumpActionSignatrue JumpAction;


private:
    ACharacter* ControlledCharacter = nullptr;
    Vector3 PendingMovementInput = Vector3(0, 0, 0);
    PxPtr<PxController> PhysController;
    
    float MaxMovementSpeed = 600.0f;
    float Speed = 0;
};
