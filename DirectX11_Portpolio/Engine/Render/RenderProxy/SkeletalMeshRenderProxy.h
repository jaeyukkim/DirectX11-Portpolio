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
    void AddInstance(USkeletalMeshComponent* meshComp);
    void DeleteInstance(const int InstanceID);
    void SetInstanceIndirectData();
    int GetNumOfInstance() const {return InstanceDatas.size();}
 
private:
    vector<FSkeletalMeshRenderData> RenderData;
    StructuredBuffer InstanceSBuffer;
    
    vector<FSKM_InstDataCPU> InstanceDatas;
    vector<IndirectBuffer> InstanceIndirectBuffer;
    
};
