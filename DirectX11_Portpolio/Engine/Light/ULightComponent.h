#pragma once

#include "Frameworks/USceneComponent.h"
#include <Render/FSceneView.h>





class ULightComponent : public USceneComponent
{
public:
    ULightComponent(ELightType InLightType);
    virtual ~ULightComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent() override;

    LightInformation* GetLightInfo() { return &LightInfo; }
private:
    
    LightInformation LightInfo;
};