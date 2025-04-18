#pragma once
#include "Frameworks/ACharacter.h"

class UCameraComponent;


class AKachujin : public ACharacter
{
public:
    AKachujin();
    virtual ~AKachujin();

public:
    virtual void Tick(float deltaTime) override;
    virtual void Render() override;
    

public:
    shared_ptr<UCameraComponent> Camera;
};
