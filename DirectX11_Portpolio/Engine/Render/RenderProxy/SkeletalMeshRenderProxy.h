#pragma once
#include "RenderProxy.h"

struct FAnimBlendingData;
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
    void UpdateAnimationData(const int InstanceID, FAnimBlendingData& blendData);
    
public:
    void CopyCntToIndirect();
    void CteateInstanceIndirectData(int instanceCnt = 0);
    void CreateCSIndirectData();
    void TransformChange(int id, Matrix& mat);
    void CalcAABB(USkeletalMeshComponent* meshComp);
    int GetNumOfInstance() const {return InstanceDatas.size();}
 
private:
    vector<FSkeletalMeshRenderData> RenderData;
    
    vector<FSKM_InstDataCPU> InstanceDatas;
    vector<IndirectBuffer> InstanceIndirectBuffer;
    IndirectBuffer CSIndirectBuffer;

    AppendBuffer Append;
    AppendBuffer Consume;
    
    Vector3 minAABB = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 maxAABB = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

};
