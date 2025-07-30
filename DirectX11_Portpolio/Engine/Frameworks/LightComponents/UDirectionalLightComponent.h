#pragma once
#include "ULightComponent.h"

class UDirectionalLightComponent : public ULightComponent
{
public:
    UDirectionalLightComponent();
    virtual void TickComponent(float deltaTime) override;


    Vector3 LightPos = Vector3(-1190, 1530, 850);
};
