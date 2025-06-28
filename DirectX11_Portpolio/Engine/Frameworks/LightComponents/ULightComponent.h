#pragma once

#include "Frameworks/Components/USceneComponent.h"


class ULightComponent : public USceneComponent
{
public:
    ULightComponent(UINT InLightType);
    virtual ~ULightComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    FLight* GetLightInfo() { return &LightInfo; }
    
protected:
    virtual void UpdateLight();
    FLight LightInfo;
};
