#pragma once
#include "ULightComponent.h"

class USpotLightComponent : public ULightComponent
{
public:
    USpotLightComponent();
    virtual void TickComponent(float deltaTime) override;
};
