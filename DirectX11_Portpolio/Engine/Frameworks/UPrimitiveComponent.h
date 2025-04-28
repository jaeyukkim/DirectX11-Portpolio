#pragma once
#include "USceneComponent.h"

class UPrimitiveComponent : public USceneComponent
{
public:
    UPrimitiveComponent() = default;
    virtual ~UPrimitiveComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent(bool bUsePreRender = false) override;


};
