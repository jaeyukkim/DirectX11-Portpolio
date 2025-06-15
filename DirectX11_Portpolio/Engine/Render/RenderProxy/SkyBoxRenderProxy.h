#pragma once
#include "RenderProxy.h"

class UStaticMeshComponent;


class SkyBoxRenderProxy : public RenderProxy
{
public:
    SkyBoxRenderProxy(UStaticMeshComponent* meshComp);
    void Render(const FRenderOption& option) override;

private:
    FSkyBoxRenderData Data;
};
