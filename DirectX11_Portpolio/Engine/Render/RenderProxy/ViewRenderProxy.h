#pragma once
#include "RenderProxy.h"


class ViewRenderProxy : RenderProxy
{
public:
    ViewRenderProxy(FSceneView* sceneView);
    void Render(const FRenderOption& option) override;
    void SetLightViewMode(UINT8 InLightID);


private:
    FViewRenderData Data;
};
