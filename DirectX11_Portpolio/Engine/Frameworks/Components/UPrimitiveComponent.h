#pragma once
#include "USceneComponent.h"

class UPrimitiveComponent : public USceneComponent
{
public:
    UPrimitiveComponent() = default;
    virtual ~UPrimitiveComponent() override = default;

public:
    virtual void TickComponent(float deltaTime) override;
    int GetInstanceID() const {return *InstanceID;}
    void SetInstanceID(int* InInstanceId) {InstanceID = InInstanceId;}

    
protected:
    PrimitiveData PrimData;
    FPrimitiveRenderData PrimRenderData;
    int* InstanceID = nullptr;

protected:
    friend class PrimitiveRenderProxy;
};
