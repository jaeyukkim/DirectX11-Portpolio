#pragma once
#include "ULightComponent.h"

class UPointLightComponent : public ULightComponent
{
public:
    UPointLightComponent();
    virtual void TickComponent(float deltaTime) override;
    
};
