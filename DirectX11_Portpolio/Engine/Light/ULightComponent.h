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

    // 기본 방향 벡터 (조명이 바라보는 기준 방향)
    Vector3 defaultDirection = Vector3(0.0f, -1.0f, 0.0f);
    LightInformation LightInfo;
};