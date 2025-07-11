#pragma once
#include "RenderProxy.h"


class UStaticMeshComponent;
class StaticMesh;



class StaticMeshRenderProxy : public RenderProxy
{
public:
    StaticMeshRenderProxy(UStaticMeshComponent* meshComp);
    void Render(const FRenderOption& option) override;
    void AddInstance(UStaticMeshComponent* meshComp);
    void DeleteInstance(const int InstanceID);
    void CopyCntToIndirect();
    void CteateInstanceIndirectData();
    void CreateCSIndirectData();
    void TransformChange(int id, Matrix& mat);
    void RunFrustumCulling();
    void CalcAABB(UStaticMeshComponent* meshComp);
    int GetNumOfInstance() const {return InstanceDatas.size();}

private:
    vector<FStaticMeshRenderData> RenderData;
    vector<FSM_InstDataCPU> InstanceDatas;

    vector<IndirectBuffer> InstanceIndirectBuffer;
    IndirectBuffer CSIndirectBuffer;
    
    Vector3 minAABB = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 maxAABB = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    AppendBuffer Append;
    AppendBuffer Consume;
};
