#pragma once
#include "RenderProxy.h"


struct FPrimitiveRenderData;


class PrimitiveRenderProxy : public RenderProxy
{
public:
    PrimitiveRenderProxy(UPrimitiveComponent* primComp);
    virtual void Render(const FRenderOption& option) override;
    void AddInstance(UPrimitiveComponent* primComp);
    void DeleteInstance(const int InstanceID);
    void TransformChange(int id, Matrix& mat);
    void CopyCntToIndirect();
    void RunFrustumCulling();
    void CteateInstanceIndirectData();
    void CreateCSIndirectData();
    
private:
    FPrimitiveRenderData RenderData;
    vector<FPrimitiveInstCPU> InstanceDatas;

    Vector3 minAABB = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 maxAABB = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    IndirectBuffer InstanceIndirectBuffer;
    IndirectBuffer CSIndirectBuffer;
    AppendBuffer Append;
    AppendBuffer Consume;
};
