#pragma once
#include "ULightComponent.h"

class UDirectionalLightComponent : public ULightComponent
{
public:
    UDirectionalLightComponent();
    virtual void TickComponent(float deltaTime) override;
};
