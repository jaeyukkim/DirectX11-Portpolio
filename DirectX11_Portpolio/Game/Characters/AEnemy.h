#pragma once
#include "Frameworks/Objects/ACharacter.h"

class AEnemy : public ACharacter
{
public:
    AEnemy();
    virtual ~AEnemy() = default;
    virtual void Tick(float deltaTime) override;
    virtual void InitEnemy();
    
};
