#include "HeaderCollection.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "StaticMeshRenderProxy.h"
#include "Render/Mesh/Buffers.h"
#include "Render/Mesh/StaticMesh.h"


StaticMeshRenderProxy::StaticMeshRenderProxy(UStaticMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_StaticMesh),
    InstanceSBuffer(0, 0, 0)
{
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
    
    AddInstance(meshComp);

    
}



void StaticMeshRenderProxy::Render(const FRenderOption& option)
{
    
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
  
    InstanceSBuffer.UpdateSubResource();
    InstanceSBuffer.VSSetStructuredBuffer(EShaderResourceSlot::ERS_InstanceData);
    for(int i = 0 ; i<RenderData.size() ; i++)
    {
        RenderData[i].MaterialData->BindMaterial();
        RenderData[i].VBuffer->IASetVertexBuffer();
        RenderData[i].IBuffer->IASetIndexBuffer();
        RenderData[i].Transform->UpdateConstBuffer();
        RenderData[i].Transform->VSSetConstantBuffer(EConstBufferSlot::CB_World, 1);
        
        
        D3D::Get()->GetDeviceContext()->DrawIndexedInstancedIndirect(InstanceIndirectBuffer[i].GetBuffer().Get(), 0);

    }
}

void StaticMeshRenderProxy::AddInstance(UStaticMeshComponent* meshComp)
{
    //여러개 메시들의 aabb를 통합된 aabb로 계산
    Vector3 minAABB = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 maxAABB = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

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

    
    FSM_InstDataCPU data;
    data.Transform = meshComp->GetWorldBufferData()->World;
    data.AABB_Max = maxAABB;
    data.AABB_Min = minAABB;
    data.InstanceID = InstanceDatas.size();
    meshComp->SetInstanceID(&data.InstanceID);
    InstanceDatas.push_back(data);
    
    SetInstanceIndirectData();

    meshComp->TransformChanged.Add([this](int id, Matrix mat)
    {
        InstanceDatas[id].Transform = mat;
       //SetInstanceIndirectData();
    });
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
    
    SetInstanceIndirectData();
}

void StaticMeshRenderProxy::SetInstanceIndirectData()
{
    InstanceIndirectBuffer.clear();
    for(FStaticMeshRenderData& data : RenderData)
    {
        D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
        ZeroMemory(&args, sizeof(args));
        args.InstanceCount = InstanceDatas.size();
        args.IndexCountPerInstance = data.IndexCount;
        args.BaseVertexLocation=0;
        args.StartIndexLocation=0;
        args.StartInstanceLocation=0;

        InstanceIndirectBuffer.push_back(IndirectBuffer(&args,
        sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS), 1));
    }

    InstanceSBuffer = StructuredBuffer(InstanceDatas.data(),
        sizeof(FSKM_InstDataCPU), InstanceDatas.size());
}
