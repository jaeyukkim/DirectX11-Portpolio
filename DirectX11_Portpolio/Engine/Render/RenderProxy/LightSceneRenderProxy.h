#pragma once
#include "RenderProxy.h"

struct FSceneView;



class LightSceneRenderProxy : public RenderProxy
{
public:
    LightSceneRenderProxy(FSceneView* sceneView);
    void Render(const FRenderOption& option) override;

private:
    FLightSceneRenderData Data;
};
