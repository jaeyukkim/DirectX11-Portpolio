#pragma once
#include "Frameworks/Actor.h"

class UStaticMeshComponent;
class AMirror : public Actor
{
public:
    AMirror();
    virtual ~AMirror() = default;

    virtual void Tick(float deltaTime) override;
    virtual void Render() override;
    virtual void PostRender() override;

public:
    shared_ptr<UStaticMeshComponent> mirror;
    DirectX::SimpleMath::Plane plane;
    Matrix ReflectRow;
};
