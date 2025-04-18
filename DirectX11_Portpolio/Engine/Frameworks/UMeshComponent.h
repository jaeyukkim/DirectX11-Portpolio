#pragma once
#include "UPrimitiveComponent.h"
#include "Render/RenderData.h"

class UMeshComponent : public UPrimitiveComponent
{
public:
    UMeshComponent() = default;
    virtual ~UMeshComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent() override;

public:

    
};

