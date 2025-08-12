#pragma once
#include "Frameworks/Objects/Actor.h"

class AController : public Actor
{
public:
    AController();
    virtual void Possess(ACharacter* InCharacter);
    virtual void UnPossess();
    virtual void Tick(float deltaTime) override;
    
public:
    virtual void AddRotationInput(Vector3 InVal);
    virtual void AddMovementInput(const Vector3& moveVal);
    virtual void Jump();
    virtual void InitPhysController();
    virtual void AddMovementToPhys(float deltaTime);
    
    inline ACharacter* GetConrolledCharacter() {return ControlledCharacter;}
    

public:
    bool IsFalling() {return bIsFalling;}
    bool IsJumping() {return bJump;}
    float GetSpeed() {return Speed;}
    Vector3 GetVelocity() {return CurrentVelocity;}


private:
    ACharacter* ControlledCharacter = nullptr;
    PxPtr<PxController> PhysController;


    
protected:
    Vector3 PendingMovementInput = Vector3(0, 0, 0);
    const float Gravity = -980.0f;
    const float JumpSpeed = 160.0f;
    float JumpTime = 0.0f;
    float InitYPos;
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
