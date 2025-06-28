#pragma once
#include "ULightComponent.h"


class UHaloLightComponent: public ULightComponent
{
public:
    UHaloLightComponent();
    virtual ~UHaloLightComponent();

public:
    virtual void TickComponent(float deltaTime) override;

};
