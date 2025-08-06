#pragma once

#include "../Combat/ICombatInterface.h"
#include "Frameworks/Objects/ACharacter.h"


class USpringArmConponent;
class UCameraComponent;
class APlayerController;

DECLARE_DYNAMIC_DELEGATE(FCanAttack);
DECLARE_DYNAMIC_DELEGATE(FCanNotAttack);
DECLARE_DYNAMIC_DELEGATE(FCanMove);
DECLARE_DYNAMIC_DELEGATE(FCanNotMove);
DECLARE_DYNAMIC_DELEGATE(FBasicAttack);


class AKachujin : public ACharacter, public ICombatInterface
{
public:
    AKachujin();
    virtual ~AKachujin();
    virtual void Possess(AController* InController) override;
    virtual void Tick(float deltaTime) override;
   
public:
    APlayerController* GetPlayerController() { return PlayerController;}
    virtual void TakeDamage(ICombatInterface* damageCauser, LL damageAmount) override;
    
protected:
    void InitMontage();
    void InitNotify();
    void LookAction(Vector3 InValue);
    void MoveCharacter(Vector2 InValue);
    void JumpCharacter();
    void Attack();
    void AbilityRMB();
    void BasicAttack();

    void CanAttack();
    void CanNotAttack();
    void CanMove();
    void CanNotMove();
    
private:
    APlayerController* PlayerController;
    vector<AnimMontage> AttackMontage;
    shared_ptr<AnimMontage> AbilityRMBMontage;
    shared_ptr<AnimMontage> HitReactMontage;
    unordered_map<string, FAnimationNotifyEvent> Notifies;
    
    
    FCanAttack CanAtkDelegate;
    FCanNotAttack CanNotAtkDelegate;
    FCanMove CanMoveDelegate;
    FCanNotMove CanNotMoveDelegate;
    FBasicAttack BasicAttackDelegate;
    

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

