#pragma once
#include "Frameworks/Manager/AController.h"

class AAIController : public AController
{
public:
    virtual void Possess(ACharacter* InCharacter);
    virtual void UnPossess();
    virtual void Tick(float deltaTime) override;
    
};
