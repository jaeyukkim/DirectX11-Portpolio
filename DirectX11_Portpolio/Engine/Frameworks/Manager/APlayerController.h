#pragma once
#include "Frameworks/Objects/Actor.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FMoveActionSignatrue, Vector2);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLookInputSignatrue, Vector3);
DECLARE_DYNAMIC_DELEGATE(FJumpActionSignatrue);


class ACharacter;



class APlayerController : public Actor
{
public:
    APlayerController();
    virtual void Possess(ACharacter* InCharacter);
    virtual void UnPossess();
    virtual void Tick(float deltaTime) override;
    
public:
    void AddRotationInput(Vector3 InVal);
    virtual void AddMovementInput(const Vector3& moveVal);
    virtual void Jump();
    
    void InitPhysController();

    
    inline ACharacter* GetConrolledCharacter() {return ControlledCharacter;}
    void ProcessMoveAction();
    void ProcessJumpAction();
    void ProcessLookInput();
    void AddMovementToPhys(float deltaTime);

public:
    bool IsFalling() {return bIsFalling;}
    bool IsJumping() {return bJump;}
    float GetSpeed() {return Speed;}
    Vector3 GetVelocity() {return CurrentVelocity;}
public:
    FMoveActionSignatrue MoveAction;
    FJumpActionSignatrue JumpAction;
    FLookInputSignatrue LookInput;

private:
    ACharacter* ControlledCharacter = nullptr;
    PxPtr<PxController> PhysController;


    
private:
    Vector3 PendingMovementInput = Vector3(0, 0, 0);
    const float Gravity = -980.0f;
    const float JumpSpeed = 80.0f;
    float RotationSpeed = 25.f;


    float VerticalVelocity = 0.0f;
    float MaxMovementSpeed = 600.0f;
    Vector3 CurrentVelocity;
    float AccelTime = 0.0f;          // 입력 유지 시간
    float DeccelTime = 0.0f;         // 감속 시간
    float Damping = 5.0f; 
    float Speed = 0;


    bool bIsFalling = false;
    bool bJump = false;
};
