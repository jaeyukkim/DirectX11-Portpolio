#include "Pch.h"
#include "AFluidActor.h"

#include "Render/Mesh/GeometryGenerator.h"

void AFluidActor::InitAllComponents()
{
    Super::InitAllComponents();
    TexTransform.SetPosition(0.0f, 300.0f, 0.0f);
    TexTransform.SetRotation(90.0f, 0.0f, 0.0f);
    
    TransformCBuffer = make_shared<ConstantBuffer>(&TransformMat, sizeof(TransformMat));
    m_StableFluid.Initialize(TexWidth, TexHeight);

    meshData = GeometryGenerator::MakeSquare(1000, Vector2(1000, 1000));
    IBuffer = make_shared<IndexBuffer>(meshData.Indices.data(), meshData.Indices.size());
    VBuffer = make_shared<VertexBuffer>(meshData.Vertices.data(), meshData.Vertices.size(), sizeof(VertexObject));

    TextureBuffer::CreateSRVUAVTexture(TexWidth, TexHeight, DXGI_FORMAT_R16G16B16A16_FLOAT,
           FloorTex.texture, FloorTex.srv, FloorTex.uav);
    
}

void AFluidActor::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    
    m_StableFluid.Render(deltaTime);
    D3D::Get()->GetDeviceContext()->CopyResource(FloorTex.texture.Get(), m_StableFluid.DensityTex.texture.Get());

    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SimpleTexturePSO);
    TransformMat = TexTransform.ToMatrix().Transpose();
    TransformCBuffer->UpdateConstBuffer();
    TransformCBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_World, 1);
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 1, FloorTex.srv.GetAddressOf());

    VBuffer->IASetVertexBuffer();
    IBuffer->IASetIndexBuffer();
    D3D::Get()->GetDeviceContext()->DrawIndexed(meshData.Indices.size(), 0, 0);

}
