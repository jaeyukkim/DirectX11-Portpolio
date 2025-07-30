#pragma once

#include "Frameworks/Objects/ACharacter.h"

class USpringArmConponent;
class UCameraComponent;


class AKachujin : public ACharacter
{
public:
    AKachujin();
    virtual ~AKachujin();
    virtual void Possess(APlayerController* InPlayerController) override;
    virtual void Tick(float deltaTime) override;



protected:
    void LookAction(Vector3 InValue);
    void MoveCharacter(Vector2 InValue);
    void JumpCharacter();


public:
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
