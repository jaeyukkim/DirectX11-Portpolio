#include "HeaderCollection.h"
#include "SkeletalMeshRenderProxy.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/Mesh/Buffers.h"

SkeletalMeshRenderProxy::SkeletalMeshRenderProxy(USkeletalMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_SkeletalMesh)
{
    for(const shared_ptr<SkeletalMesh>& mesh : meshComp->GetAllMeshes())
    {
        FSkeletalMeshRenderData data;
        data.VBuffer = mesh->VBuffer;
        data.IBuffer = mesh->IBuffer;
        data.BoneBuffer = mesh->BoneBuffer;
        data.MaterialData = mesh->GetMaterialData();
        data.IndexCount = mesh->Data.IndexCount;
        data.Transform = meshComp->GetWorldConstantBuffer();
        Data.push_back(data);
    }
    
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
    
    
    for(FSkeletalMeshRenderData& data : Data)
    {
        
        data.BoneBuffer->UpdateConstBuffer();
        data.BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::Bone, 1);
        data.MaterialData->BindMaterial();
        data.VBuffer->IASetVertexBuffer();
        data.IBuffer->IASetIndexBuffer();
        data.Transform->VSSetConstantBuffer(EConstBufferSlot::World, 1);
        data.Transform->UpdateConstBuffer();
        D3D::Get()->GetDeviceContext()->DrawIndexed(data.IndexCount, 0, 0);
    }
    
}
