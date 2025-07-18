#include "HeaderCollection.h"
#include "SkeletalMeshRenderProxy.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/Mesh/Buffers.h"


SkeletalMeshRenderProxy::SkeletalMeshRenderProxy(USkeletalMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_SkeletalMesh),
    Append(nullptr, sizeof(FSKM_InstDataCPU), 0),
    Consume(nullptr, sizeof(FSKM_InstDataCPU), 0),
    CSIndirectBuffer(nullptr, 0, 0)
{
    // ������ ��Ҹ� �ٸ������� �������̴� �ݵ�� �����������.
    // �������� ������ capacity �ʰ��� ���� reallocation���� �������� ������ �� ��ȿȭ��
    InstanceDatas.reserve(MAX_INSTANCE_SIZE);

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
    
    CalcAABB(meshComp);
    AddInstance(meshComp);
    
}


void SkeletalMeshRenderProxy::RunFrustumCulling()
{
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->FrustumCullingPSO);
    
    Append = AppendBuffer(nullptr, sizeof(FSKM_InstDataCPU), InstanceDatas.size());
    //Consume = AppendBuffer(InstanceDatas.data(), sizeof(FSKM_InstDataCPU), InstanceDatas.size());
    Consume.UpdateSubResource();
  
    Consume.CSSetUAV(EUAV_Slot::USLOT_InstanceConsume, InstanceDatas.size());
    Append.CSSetUAV(EUAV_Slot::USLOT_InstanceAppend, 0);
    
    D3D::Get()->GetDeviceContext()->DispatchIndirect(CSIndirectBuffer.GetBuffer().Get(), 0);
    

    D3D::Get()->ComputeShaderBarrier();
    
    CopyCntToIndirect();
}


void SkeletalMeshRenderProxy::Render(const FRenderOption& option)
{

    if(option.bDepthOnly)
    {
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->DepthOnlySkinnedPSO);
    }
    else
    {
        RunFrustumCulling();
    }
    
    // Todo : Enum ���� ��Ʈ����ŷ���� ���� �������� �����ϰ� ���� ����
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
    
    CopyCntToIndirect();

    if(option.bDepthOnly)
    {
        Consume.UpdateSubResource();
        Consume.VSSetSRV(EShaderResourceSlot::ERS_InstanceData);
    }
    else
    {
        Append.VSSetSRV(EShaderResourceSlot::ERS_InstanceData);
    }
    
    
    for(int i = 0 ; i<RenderData.size() ; i++)
    {
        RenderData[i].BoneBuffer->UpdateConstBuffer();
        RenderData[i].BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_Bone, 1);
        RenderData[i].MaterialData->BindMaterial();
        RenderData[i].VBuffer->IASetVertexBuffer();
        RenderData[i].IBuffer->IASetIndexBuffer();
        
        if(!option.bDepthOnly)
            D3D::Get()->GetDeviceContext()->DrawIndexedInstancedIndirect(InstanceIndirectBuffer[i].GetBuffer().Get(), 0);
        else
        {
            D3D::Get()->GetDeviceContext()->DrawIndexed(RenderData[i].IndexCount, 0, 0);
        }
    }
}


void SkeletalMeshRenderProxy::AddInstance(USkeletalMeshComponent* meshComp)
{
    if (InstanceDatas.size() > MAX_INSTANCE_SIZE)
    {
        Assert(true, "�ִ� Instance ���� �ʰ��Ͽ����ϴ�.");
    }


    FSKM_InstDataCPU data;
    data.ModelMat = meshComp->GetWorldBufferData()->World;
    data.AABB_Max = maxAABB;
    data.AABB_Min = minAABB;
    data.InstanceID = InstanceDatas.size();
    InstanceDatas.push_back(data);

    meshComp->SetInstanceID(&InstanceDatas.back().InstanceID);

    Consume = AppendBuffer(InstanceDatas.data(),
        sizeof(FSKM_InstDataCPU), InstanceDatas.size());

    //�ʱ� ������� ����������Ѵٰ�??
    Append = AppendBuffer(nullptr, sizeof(FSKM_InstDataCPU), 1);

    
    meshComp->TransformChanged.Add(this, &SkeletalMeshRenderProxy::TransformChange);

    CreateCSIndirectData();
    CteateInstanceIndirectData();
   
}


void SkeletalMeshRenderProxy::DeleteInstance(const int InstanceID)
{
    if (InstanceID < 0 || InstanceID >= InstanceDatas.size()) return;
    InstanceDatas.erase(InstanceDatas.begin() + InstanceID);

    //instanceID �ʱ�ȭ
    for(int i = 0 ; i<InstanceDatas.size() ; i++)
    {
        InstanceDatas[i].InstanceID = i;
    }
    

    Consume = AppendBuffer(InstanceDatas.data(),
        sizeof(FSKM_InstDataCPU), InstanceDatas.size());
    Append = AppendBuffer(nullptr, sizeof(FSKM_InstDataCPU), 1);
 
    CreateCSIndirectData();
    CteateInstanceIndirectData();
}

void SkeletalMeshRenderProxy::UpdateAnimationData(const int InstanceID, FAnimBlendingData& blendData)
{
    InstanceDatas[InstanceID].AnimBlendData = blendData;
}


void SkeletalMeshRenderProxy::CopyCntToIndirect()
{
    
    for(IndirectBuffer& buffer : InstanceIndirectBuffer)
    {
        // Append�� ī���� ���� IndirectBuffer�� InstanceCount ��ġ�� GPU �� GPU�� ����
        // AppendUAV �� IndirectBuffer �� offset 4 ����Ʈ ��ġ (InstanceCount)
         D3D::Get()->GetDeviceContext()->CopyStructureCount(buffer.GetBuffer().Get(), 4, Append.GetUAV().Get());
    }
    
}


void SkeletalMeshRenderProxy::CteateInstanceIndirectData(int instanceCnt)
{
    InstanceIndirectBuffer.clear();
    for(FSkeletalMeshRenderData& data : RenderData)
    {
        D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
        ZeroMemory(&args, sizeof(args));
        args.InstanceCount = instanceCnt;
        args.IndexCountPerInstance = data.IndexCount;
        args.BaseVertexLocation=0;
        args.StartIndexLocation=0;
        args.StartInstanceLocation=0;
        
        InstanceIndirectBuffer.push_back(IndirectBuffer(&args,
        sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS), 1));
    }
}


void SkeletalMeshRenderProxy::CreateCSIndirectData()
{
    D3D11_DISPATCH_INDIRECT_ARGS args;
    args.ThreadGroupCountX = InstanceDatas.size();
    args.ThreadGroupCountY = 1;
    args.ThreadGroupCountZ = 1;

    CSIndirectBuffer = IndirectBuffer(&args,
        sizeof(D3D11_DISPATCH_INDIRECT_ARGS), 1);

    CSIndirectBuffer.UpdateSubResource();
}


void SkeletalMeshRenderProxy::TransformChange(int id, Matrix& mat)
{
    if (id >= 0 && id <= InstanceDatas.size()-1)
    {
        InstanceDatas[id].ModelMat = mat;
    }
}


void SkeletalMeshRenderProxy::CalcAABB(USkeletalMeshComponent* meshComp)
{
    //������ �޽õ��� aabb�� ���յ� aabb�� ���
    

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
}