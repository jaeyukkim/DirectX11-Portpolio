#include "HeaderCollection.h"
#include "SkyBoxRenderProxy.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "Render/Mesh/StaticMesh.h"


SkyBoxRenderProxy::SkyBoxRenderProxy(UStaticMeshComponent* meshComp)
    :RenderProxy(RPT_SkyBox)
{
    for(const shared_ptr<StaticMesh>& mesh : meshComp->GetAllMeshes())
    {
        Data.VBuffer = mesh->VBuffer;
        Data.IBuffer = mesh->IBuffer;
        Data.MaterialData = mesh->GetMaterialData();
        Data.IndexCount = mesh->Data.IndexCount;
        Data.Transform = meshComp->GetWorldConstantBuffer();
    }
}

void SkyBoxRenderProxy::Render(const FRenderOption& option)
{
    
    if(option.bDefaultDraw)
    {
        if (!option.bIsMirror)
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SkyboxSolidPSO);
        }
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->ReflectSkyboxSolidPSO);
        }
    }
    else if(option.bIsWire)
    {
        if (!option.bIsMirror)
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SkyboxWirePSO);
        }
        else
        {
            FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->ReflectSkyboxWirePSO);
        }
    }
    
    Data.MaterialData->BindMaterial();
    Data.VBuffer->IASetVertexBuffer();
    Data.IBuffer->IASetIndexBuffer();
    Data.Transform->VSSetConstantBuffer(EConstBufferSlot::World, 1);
    Data.Transform->UpdateConstBuffer();
    D3D::Get()->GetDeviceContext()->DrawIndexed(Data.IndexCount, 0, 0);

}
