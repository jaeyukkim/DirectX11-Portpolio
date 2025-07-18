#include "HeaderCollection.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "StaticMeshRenderProxy.h"
#include "Render/Mesh/Buffers.h"
#include "Render/Mesh/StaticMesh.h"


StaticMeshRenderProxy::StaticMeshRenderProxy(UStaticMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_StaticMesh),
    Append(nullptr, sizeof(FSKM_InstDataCPU), 0),
    Consume(nullptr, sizeof(FSKM_InstDataCPU), 0),
    CSIndirectBuffer(nullptr, 0, 0)
{

    // 벡터의 요소를 다른곳에서 참조중이니 반드시 지정해줘야함.
    // 지정하지 않으면 capacity 초과로 인한 reallocation으로 참조중인 포인터 값 무효화됨
    InstanceDatas.reserve(MAX_INSTANCE_SIZE);

    for(const shared_ptr<StaticMesh>& mesh : meshComp->GetAllMeshes())
    {
        FStaticMeshRenderData data;
        data.VBuffer = mesh->VBuffer;
        data.IBuffer = mesh->IBuffer;
        data.MaterialData = mesh->GetMaterialData();
        data.IndexCount = mesh->Data.IndexCount;
        data.Transform = meshComp->GetWorldConstantBuffer();
        RenderData.push_back(data);
    }

    CalcAABB(meshComp);
    AddInstance(meshComp);

    
}

void StaticMeshRenderProxy::RunFrustumCulling()
{
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->FrustumCullingPSO);
    
    Append = AppendBuffer(nullptr, sizeof(FSKM_InstDataCPU), 1);
    //Consume = AppendBuffer(InstanceDatas.data(), sizeof(FSKM_InstDataCPU), InstanceDatas.size());
    Consume.UpdateSubResource();
  
    Consume.CSSetUAV(EUAV_Slot::USLOT_InstanceConsume, InstanceDatas.size());
    Append.CSSetUAV(EUAV_Slot::USLOT_InstanceAppend, 0);
    
    D3D::Get()->GetDeviceContext()->DispatchIndirect(CSIndirectBuffer.GetBuffer().Get(), 0);
    D3D::Get()->ComputeShaderBarrier();
    
    CopyCntToIndirect();
}

void StaticMeshRenderProxy::Render(const FRenderOption& option)
{
    if(option.bDepthOnly)
    {
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->DepthOnlyPSO);
    }
    else
    {
        RunFrustumCulling();
    }
    
    // Todo : Enum 만들어서 비트마스킹으로 여러 조합으로 가능하게 만들 예정
    if(option.bDefaultDraw)
    {
        if(!option.bIsMirror)
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshSolidPSO);
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshReflectSolidPSO);
        }
    }
    else if(option.bIsWire)
    {
        if(!option.bIsMirror)
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshWirePSO);
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshReflectWirePSO);
        }
    }
    else if(option.bStencilOn)
    {
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->StencilMaskPSO);
    }
    else if(option.bBlendOn)
    {
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MirrorBlendSolidPSO);
    }

    
    CopyCntToIndirect();
    Append.VSSetSRV(EShaderResourceSlot::ERS_InstanceData);
    for(int i = 0 ; i<RenderData.size() ; i++)
    {
        RenderData[i].MaterialData->BindMaterial();
        RenderData[i].VBuffer->IASetVertexBuffer();
        RenderData[i].IBuffer->IASetIndexBuffer();
        RenderData[i].Transform->UpdateConstBuffer();
        RenderData[i].Transform->VSSetConstantBuffer(EConstBufferSlot::CB_World, 1);
        
        if(!option.bDepthOnly)
            D3D::Get()->GetDeviceContext()->DrawIndexedInstancedIndirect(InstanceIndirectBuffer[i].GetBuffer().Get(), 0);
        else
        {
            D3D::Get()->GetDeviceContext()->DrawIndexed(RenderData[i].IndexCount, 0, 0);
        }
    }
}

void StaticMeshRenderProxy::AddInstance(UStaticMeshComponent* meshComp)
{
    if (InstanceDatas.size() > MAX_INSTANCE_SIZE)
    {
        Assert(true, "최대 Instance 수를 초과하였습니다.");
    }

    
    FSM_InstDataCPU data;
    data.ModelMat = meshComp->GetWorldBufferData()->World;
    data.AABB_Max = maxAABB;
    data.AABB_Min = minAABB;
    data.InstanceID = InstanceDatas.size();
    InstanceDatas.push_back(data);

    meshComp->SetInstanceID(&InstanceDatas.back().InstanceID);

    Consume = AppendBuffer(InstanceDatas.data(),
        sizeof(FSKM_InstDataCPU), InstanceDatas.size());

    //초기 사이즈는 지정해줘야한다고??
    Append = AppendBuffer(nullptr, sizeof(FSKM_InstDataCPU), 1);

    
    meshComp->TransformChanged.Add(this, &StaticMeshRenderProxy::TransformChange);

    CreateCSIndirectData();
    CteateInstanceIndirectData();
}


/**
 * @param InstanceID Instance의 ID
 */
void StaticMeshRenderProxy::DeleteInstance(const int InstanceID)
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
    Append = AppendBuffer(nullptr, sizeof(FSKM_InstDataCPU), 1);
 
    CreateCSIndirectData();
    CteateInstanceIndirectData();

}

void StaticMeshRenderProxy::CopyCntToIndirect()
{
    for(IndirectBuffer& buffer : InstanceIndirectBuffer)
    {
        // Append의 카운터 값만 IndirectBuffer의 InstanceCount 위치에 GPU → GPU로 복사
        // AppendUAV → IndirectBuffer 의 offset 4 바이트 위치 (InstanceCount)
        D3D::Get()->GetDeviceContext()->CopyStructureCount(buffer.GetBuffer().Get(), 4, Append.GetUAV().Get());
    }
}

void StaticMeshRenderProxy::CteateInstanceIndirectData()
{
    InstanceIndirectBuffer.clear();
    for(FStaticMeshRenderData& data : RenderData)
    {
        D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
        ZeroMemory(&args, sizeof(args));
        args.InstanceCount = 0;
        args.IndexCountPerInstance = data.IndexCount;
        args.BaseVertexLocation=0;
        args.StartIndexLocation=0;
        args.StartInstanceLocation=0;
        
        InstanceIndirectBuffer.push_back(IndirectBuffer(&args,
        sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS), 1));
    }
}

void StaticMeshRenderProxy::CreateCSIndirectData()
{
    D3D11_DISPATCH_INDIRECT_ARGS args;
    args.ThreadGroupCountX = InstanceDatas.size();
    args.ThreadGroupCountY = 1;
    args.ThreadGroupCountZ = 1;

    CSIndirectBuffer = IndirectBuffer(&args,
        sizeof(D3D11_DISPATCH_INDIRECT_ARGS), 1);
}


void StaticMeshRenderProxy::TransformChange(int id, Matrix& mat)
{
    InstanceDatas[id].ModelMat = mat;
}


void StaticMeshRenderProxy::CalcAABB(UStaticMeshComponent* meshComp)
{
    //여러개 메시들의 aabb를 통합된 aabb로 계산


    for (const shared_ptr<StaticMesh>& mesh : meshComp->GetAllMeshes())
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