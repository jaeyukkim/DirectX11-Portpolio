#pragma once

#include "Frameworks/Components/USceneComponent.h"


class ULightComponent : public USceneComponent
{
public:
    ULightComponent(UINT InLightType);
    virtual ~ULightComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    FLightInformation* GetLightInfo() { return &LightInfo; }
    
protected:
    virtual void UpdateLight();
    FLightInformation LightInfo;
};
