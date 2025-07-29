#pragma once
#include "Frameworks/Animation/UAnimInstance.h"


class APlayerController;
class AKachujin;

class UMyAnimInstance : public UAnimInstance
{
public:
    UMyAnimInstance(USkeletalMeshComponent* meshComp);
    virtual ~UMyAnimInstance() = default;

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float deltaTime);
 
    void CreateNode();

private:
    float Speed = 0.0f;
    Vector3 Direction;
    bool bFalling = false;
    
    
private:
    AKachujin* Paladin = nullptr;
    APlayerController* Controller = nullptr;
};
