#pragma once
#include "RenderProxy.h"


class ViewRenderProxy : RenderProxy
{
public:
    ViewRenderProxy(FSceneView* sceneView);
    void Render(const FRenderOption& option) override;


private:
    FViewRenderData Data;
};
