#include "HeaderCollection.h"
#include "SkeletalMeshRenderProxy.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/Mesh/Buffers.h"

SkeletalMeshRenderProxy::SkeletalMeshRenderProxy(USkeletalMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_SkeletalMesh),
    Append(nullptr, 0, 0),
    Consume(nullptr, 0, 0),
    CSIndirectBuffer(nullptr, 0, 0)
{
    for(const shared_ptr<SkeletalMesh>& mesh : meshComp->GetAllMeshes())
    {
        FSkeletalMeshRenderData data;
        data.VBuffer = mesh->VBuffer;
        data.IBuffer = mesh->IBuffer;
        data.BoneBuffer = mesh->BoneBuffer;
        data.MaterialData = mesh->GetMaterialData();
        data.IndexCount = mesh->Data.IndexCount;
        RenderData.push_back(data);
    }
    
    AddInstance(meshComp);

    
}


void SkeletalMeshRenderProxy::RunFrustumCulling()
{
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->FrustumCullingPSO);
    
    Consume = AppendBuffer(InstanceDatas.data(),
        sizeof(FSKM_InstDataCPU), InstanceDatas.size());
    Consume.CSSetUAV(EUAV_Slot::USLOT_InstanceConsume, InstanceDatas.size());
    
   
    Append.CSSetUAV(EUAV_Slot::USLOT_InstanceAppend, 0);
    SetCSIndirectData();
    
    D3D::Get()->GetDeviceContext()->DispatchIndirect(CSIndirectBuffer.GetBuffer().Get(), 0);
    D3D::Get()->ComputeShaderBarrier();
    
}


void SkeletalMeshRenderProxy::Render(const FRenderOption& option)
{

    RunFrustumCulling();
   
    // Todo : Enum 만들어서 비트마스킹으로 여러 조합으로 가능하게 만들 예정
    if(option.bDefaultDraw)
    {
        if(!option.bIsMirror)
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SkeletalMeshSolidPSO);
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SkeletalMeshReflectSolidPSO);
        }
    }
    else if(option.bIsWire)
    {
        if(!option.bIsMirror)
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SkeletalMeshWirePSO);
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SkeletalMeshReflectWirePSO);
        }
    }
    
    
    
    for(int i = 0 ; i<RenderData.size() ; i++)
    {
        RenderData[i].BoneBuffer->UpdateConstBuffer();
        RenderData[i].BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_Bone, 1);
        RenderData[i].MaterialData->BindMaterial();
        RenderData[i].VBuffer->IASetVertexBuffer();
        RenderData[i].IBuffer->IASetIndexBuffer();
        
        //InstanceIndirectBuffer[i].UpdateSubResource();
        D3D::Get()->GetDeviceContext()->DrawIndexedInstancedIndirect(InstanceIndirectBuffer[i].GetBuffer().Get(), 0);
    }
}




void SkeletalMeshRenderProxy::AddInstance(USkeletalMeshComponent* meshComp)
{

    //여러개 메시들의 aabb를 통합된 aabb로 계산
    Vector3 minAABB = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 maxAABB = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const shared_ptr<SkeletalMesh>& mesh : meshComp->GetAllMeshes())
    {
        const Vector3& AABBMin = mesh->Data.AABB.Min;
        const Vector3& AABBMax = mesh->Data.AABB.Max;

        minAABB.x = std::min(minAABB.x, AABBMin.x);
        minAABB.y = std::min(minAABB.y, AABBMin.y);
        minAABB.z = std::min(minAABB.z, AABBMin.z);

        maxAABB.x = max(maxAABB.x, AABBMax.x);
        maxAABB.y = max(maxAABB.y, AABBMax.y);
        maxAABB.z = max(maxAABB.z, AABBMax.z);
    }

    
    FSKM_InstDataCPU data;
    data.ModelMat = meshComp->GetWorldBufferData()->World;
    data.AABB_Max = maxAABB;
    data.AABB_Min = minAABB;
    data.InstanceID = InstanceDatas.size();
    meshComp->SetInstanceID(&data.InstanceID);
    InstanceDatas.push_back(data);
    
    
    meshComp->TransformChanged.Add([this](int id, Matrix& mat)
    {
        InstanceDatas[id].ModelMat = mat;
    });
}


void SkeletalMeshRenderProxy::DeleteInstance(const int InstanceID)
{
    if (InstanceID < 0 || InstanceID >= InstanceDatas.size()) return;
    InstanceDatas.erase(InstanceDatas.begin() + InstanceID);

    //instanceID 초기화
    for(int i = 0 ; i<InstanceDatas.size() ; i++)
    {
        InstanceDatas[i].InstanceID = i;
    }

    Consume = AppendBuffer(InstanceDatas.data(),
        sizeof(FSKM_InstDataCPU), InstanceDatas.size());
}


void SkeletalMeshRenderProxy::SetInstanceIndirectData(int InSize)
{
    
    InstanceIndirectBuffer.clear();
    for(FSkeletalMeshRenderData& data : RenderData)
    {
        D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
        ZeroMemory(&args, sizeof(args));
        args.InstanceCount = InSize;
        args.IndexCountPerInstance = data.IndexCount;
        args.BaseVertexLocation=0;
        args.StartIndexLocation=0;
        args.StartInstanceLocation=0;
        
        InstanceIndirectBuffer.push_back(IndirectBuffer(&args,
        sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS), 1));
    }

    

}

void SkeletalMeshRenderProxy::SetCSIndirectData()
{
    D3D11_DISPATCH_INDIRECT_ARGS args;
    args.ThreadGroupCountX = InstanceDatas.size();
    args.ThreadGroupCountY = 1;
    args.ThreadGroupCountZ = 1;

    CSIndirectBuffer = IndirectBuffer(&args,
        sizeof(D3D11_DISPATCH_INDIRECT_ARGS), 1);
}
