#pragma once
#include "RenderProxy.h"

class IndirectBuffer;
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

    void RunFrustumCulling();
    void AddInstance(USkeletalMeshComponent* meshComp);
    void DeleteInstance(const int InstanceID);
    void SetInstanceIndirectData(int InSize);
    void SetCSIndirectData();
    int GetNumOfInstance() const {return InstanceDatas.size();}
 
private:
    vector<FSkeletalMeshRenderData> RenderData;
    
    vector<FSKM_InstDataCPU> InstanceDatas;
    vector<IndirectBuffer> InstanceIndirectBuffer;
    IndirectBuffer CSIndirectBuffer;

    AppendBuffer Append;
    AppendBuffer Consume;
    
};
