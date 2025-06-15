#pragma once
#include "Frameworks/Objects/Actor.h"

class UStaticMeshComponent;
class AMirror : public Actor
{
public:
    AMirror();
    virtual ~AMirror() = default;

    virtual void Tick(float deltaTime) override;


private:
    shared_ptr<UStaticMeshComponent> mirror;
    DirectX::SimpleMath::Plane plane;
    Matrix ReflectRow;
    array<float, 4> BlendFactor = { 0.93f, 0.93f, 0.93f, 1.0f };

    friend class MirrorRenderProxy;
};
