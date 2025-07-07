#include "HeaderCollection.h"
#include "SkeletalMeshRenderProxy.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/Mesh/Buffers.h"

SkeletalMeshRenderProxy::SkeletalMeshRenderProxy(USkeletalMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_SkeletalMesh),
    InstanceSBuffer(0, 0, 0)
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


void SkeletalMeshRenderProxy::Render(const FRenderOption& option)
{
    
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
    
    
    InstanceSBuffer.UpdateSubResource();
    InstanceSBuffer.VSSetStructuredBuffer(EShaderResourceSlot::ERS_World);
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
    FSKM_InstDataCPU data;
    data.Transform = meshComp->GetWorldBufferData()->World;
    meshComp->SetInstanceID(InstanceDatas.size());
    InstanceDatas.push_back(data);
    
    SetInstanceIndirectData();

    meshComp->TransformChanged.Add([this](int id, Matrix& mat)
    {
        InstanceDatas[id].Transform = mat;
       //SetInstanceIndirectData();
    });
}


void SkeletalMeshRenderProxy::DeleteInstance(const int InstanceID)
{
    if (InstanceID < 0 || InstanceID >= InstanceDatas.size()) return;
    InstanceDatas.erase(InstanceDatas.begin() + InstanceID);
    
    SetInstanceIndirectData();
}


void SkeletalMeshRenderProxy::SetInstanceIndirectData()
{
    
    InstanceIndirectBuffer.clear();
    for(FSkeletalMeshRenderData& data : RenderData)
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
