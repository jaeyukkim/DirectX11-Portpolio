#include "HeaderCollection.h"
#include "Render/Mesh/GeometryGenerator.h"

#include "PostEffect.h"

PostEffect::PostEffect()
{
    Initialize();
}

void PostEffect::Initialize()
{
    PostEffectCBuffer = make_shared<ConstantBuffer>(&Data, sizeof(PostEffectsData));
    
    MeshData = make_shared<VertexTextureData>(GeometryGenerator::MakeScreen());
    VBuffer = make_shared<VertexBuffer>(MeshData->Data.data(), MeshData->Data.size(), sizeof(VertexTexture));
    IdxCount = MeshData->Indices.size();
    IBuffer = make_shared<IndexBuffer>(MeshData->Indices.data(), IdxCount);
}

void PostEffect::Render()
{
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->PostEffectsPSO);

    PostEffectsSRVs = { D3D::Get()->ResolvedSRV.Get(), D3D::Get()->DepthOnlySRV.Get() };
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::ERS_PostEffect),
                                      UINT(PostEffectsSRVs.size()), PostEffectsSRVs.data());
    
    D3D::Get()->GetDeviceContext()->OMSetRenderTargets(1, D3D::Get()->PostEffectRTV.GetAddressOf(), NULL);
   
    PostEffectCBuffer->UpdateConstBuffer();
    PostEffectCBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_PostEffectData, 1);

    VBuffer->IASetVertexBuffer();
    IBuffer->IASetIndexBuffer();
    D3D::Get()->GetDeviceContext()->DrawIndexed(IdxCount, 0, 0);

}