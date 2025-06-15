#pragma once
#include "RenderProxy.h"


class UStaticMeshComponent;
class StaticMesh;



class StaticMeshRenderProxy : public RenderProxy
{
public:
    StaticMeshRenderProxy(UStaticMeshComponent* meshComp);
    void Render(const FRenderOption& option) override;

private:
    vector<FStaticMeshRenderData> Data;

};
