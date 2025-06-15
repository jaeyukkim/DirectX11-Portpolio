#include "HeaderCollection.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "StaticMeshRenderProxy.h"
#include "Render/Mesh/Buffers.h"
#include "Render/Mesh/StaticMesh.h"


StaticMeshRenderProxy::StaticMeshRenderProxy(UStaticMeshComponent* meshComp)
    :RenderProxy(ERenderProxyType::RPT_StaticMesh)
{
    
    for(const shared_ptr<StaticMesh>& mesh : meshComp->GetAllMeshes())
    {
        FStaticMeshRenderData data;
        data.VBuffer = mesh->VBuffer;
        data.IBuffer = mesh->IBuffer;
        data.MaterialData = mesh->GetMaterialData();
        data.IndexCount = mesh->Data.IndexCount;
        data.Transform = meshComp->GetWorldConstantBuffer();
        Data.push_back(data);
    }
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
  
    
    for(FStaticMeshRenderData& data : Data)
    {
        data.MaterialData->BindMaterial();
        data.VBuffer->IASetVertexBuffer();
        data.IBuffer->IASetIndexBuffer();
        data.Transform->UpdateConstBuffer();
        data.Transform->VSSetConstantBuffer(EConstBufferSlot::World, 1);
        D3D::Get()->GetDeviceContext()->DrawIndexed(data.IndexCount, 0, 0);
    }
}
