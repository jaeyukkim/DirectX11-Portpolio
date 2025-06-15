#pragma once
#include "RenderProxy.h"

class AMirror;
class UStaticMeshComponent;

class MirrorRenderProxy : public RenderProxy
{
public:
    MirrorRenderProxy(AMirror* mirror);
    void Render(const FRenderOption& option) override;

    Matrix GetReflactRow() const { return *Data.ReflectionRow; }

private:
    FMirrorRenderData Data;
    
};
