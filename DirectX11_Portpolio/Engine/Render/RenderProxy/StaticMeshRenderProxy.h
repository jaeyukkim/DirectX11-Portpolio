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
    void SetInstanceIndirectData();
    int GetNumOfInstance() const {return InstanceDatas.size();}

private:
    vector<FStaticMeshRenderData> RenderData;
    vector<IndirectBuffer> InstanceIndirectBuffer;

    StructuredBuffer InstanceSBuffer;
    vector<FSM_InstDataCPU> InstanceDatas;
};
