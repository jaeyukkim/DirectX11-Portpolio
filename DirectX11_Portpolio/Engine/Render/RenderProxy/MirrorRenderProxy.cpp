#include "HeaderCollection.h"
#include "MirrorRenderProxy.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "../../../Game/Enviroment/AMirror.h"
#include "Render/Mesh/StaticMesh.h"

MirrorRenderProxy::MirrorRenderProxy(AMirror* mirror)
    :RenderProxy(RPT_Mirror)
{
    UStaticMeshComponent* meshComp = mirror->mirror.get();
    
    for(const shared_ptr<StaticMesh>& mesh : meshComp->GetAllMeshes())
    {
        Data.MeshData.VBuffer = mesh->VBuffer;
        Data.MeshData.IBuffer = mesh->IBuffer;
        Data.MeshData.MaterialData = mesh->GetMaterialData();
        Data.MeshData.IndexCount = mesh->Data.IndexCount;
        Data.MeshData.Transform = meshComp->GetWorldConstantBuffer();
        Data.ReflectionRow = &mirror->ReflectRow;
        Data.BlendFactor = &mirror->BlendFactor;
    }
}


void MirrorRenderProxy::Render(const FRenderOption& option)
{
    
    // Todo : Enum 만들어서 비트마스킹으로 여러 조합으로 가능하게 만들 예정
    if(option.bDefaultDraw)
    {
        if (!option.bIsMirror)
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshSolidPSO);
        }
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshReflectSolidPSO);
        }
    }
    else if(option.bIsWire)
    {
        if (!option.bIsMirror)
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->MeshWirePSO);
        }
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
        FPSO& BlendPSO = FGlobalPSO::Get()->MirrorBlendSolidPSO;
        BlendPSO.SetBlendFactor(*Data.BlendFactor);
        FGlobalPSO::Get()->BindPSO(BlendPSO);
    }
    else if(option.bDepthOnly)
    {
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->DepthOnlyPSO);
    }
  
    
    Data.MeshData.MaterialData->BindMaterial();
    Data.MeshData.VBuffer->IASetVertexBuffer();
    Data.MeshData.IBuffer->IASetIndexBuffer();
    Data.MeshData.Transform->VSSetConstantBuffer(EConstBufferSlot::CB_World, 1);
    Data.MeshData.Transform->UpdateConstBuffer();
    D3D::Get()->GetDeviceContext()->DrawIndexed(Data.MeshData.IndexCount, 0, 0);
}
