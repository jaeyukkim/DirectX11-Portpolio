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
private:

    // �⺻ ���� ���� (������ �ٶ󺸴� ���� ����)
    Vector3 defaultDirection = Vector3(0.0f, -1.0f, 0.0f);
    FLightInformation LightInfo;
};
