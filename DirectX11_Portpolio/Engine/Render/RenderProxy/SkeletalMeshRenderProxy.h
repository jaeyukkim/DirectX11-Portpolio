#pragma once
#include "RenderProxy.h"

class USkeletalMeshComponent;
class SkeletalMesh;
class ConstantBuffer;
class VertexBuffer;
class IndexBuffer;


class SkeletalMeshRenderProxy : public RenderProxy
{
public:
    SkeletalMeshRenderProxy(USkeletalMeshComponent* meshComp);
    void Render(const FRenderOption& option) override;

private:
    vector<FSkeletalMeshRenderData> Data;
};
