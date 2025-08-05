#pragma once

#include "Frameworks/Objects/ACharacter.h"
#include "Player/UMyAnimInstance.h"

class USpringArmConponent;
class UCameraComponent;

DECLARE_DYNAMIC_DELEGATE(FCanAttack);
DECLARE_DYNAMIC_DELEGATE(FCanNotAttack);
DECLARE_DYNAMIC_DELEGATE(FCanMove);
DECLARE_DYNAMIC_DELEGATE(FCanNotMove);


class AKachujin : public ACharacter
{
public:
    AKachujin();
    virtual ~AKachujin();
    virtual void Possess(APlayerController* InPlayerController) override;
    virtual void Tick(float deltaTime) override;



protected:
    void InitMontage();
    void InitNotify();
    void LookAction(Vector3 InValue);
    void MoveCharacter(Vector2 InValue);
    void JumpCharacter();
    void Attack();
    void AbilityRMB();

    void CanAttack();
    void CanNotAttack();
    void CanMove();
    void CanNotMove();
private:
    vector<AnimMontage> AttackMontage;
    shared_ptr<AnimMontage> AbilityRMBMontage;
    unordered_map<string, FAnimationNotifyEvent> Notifies;
    
    
    FCanAttack CanAtkDelegate;
    FCanNotAttack CanNotAtkDelegate;
    FCanMove CanMoveDelegate;
    FCanNotMove CanNotMoveDelegate;

    uint8 MaxAttackCount = 0;
    uint8 AttackCount = 0;
    bool bCanAttack = true;
    bool bCanMove = true;
    
public:
    float RotateSpeed = 5.0f;
    float Roughness = 1.0f;     //4
    float Metallic = 1.0f;      //4

    int bUseAlbedoMap = false;  //4
    int bUseNormalMap = false;  //4
    int bUseAOMap = false;  //4
    int bInvertNormalMapY = false;  //4
    int bUseMetallicMap = false;    //4
    int bUseRoughnessMap = false;    //4
    int bUseEmissiveMap = false;    //4
    
public:
    shared_ptr<UCameraComponent> Camera;
    shared_ptr<USpringArmConponent> SpringArm;
};

