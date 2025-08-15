#include "Pch.h"
#include "ACloud.h"
#include "Frameworks/Collision/UBoxComponent.h"
#include "Render/FSceneRender.h"
#include "Render/Mesh/GeometryGenerator.h"

ACloud::ACloud()
{
    boxCollision = CreateComponent<UBoxComponent>(this, Vector3(500.0f, 500.0f, 500.0f));
    SetRootComponent(boxCollision.get());

    meshData = GeometryGenerator::MakeSquare(250);
    IBuffer = make_shared<IndexBuffer>(meshData.Indices.data(), meshData.Indices.size());
    VBuffer = make_shared<VertexBuffer>(meshData.Vertices.data(), meshData.Vertices.size(), sizeof(VertexObject));

    densityTex.Initialize(m_volumeWidth, m_volumeHeight, m_volumeDepth,
        DXGI_FORMAT_R16_FLOAT, {});
    lightingTex.Initialize(m_lightWidth, m_lightHeight, m_lightDepth,
        DXGI_FORMAT_R16_FLOAT, {});
    m_volumeConstsGpu = make_shared<ConstantBuffer>(&m_volumeConstsCpu, sizeof(VolumeConsts));

    FGlobalPSO::Get()->CompileCS(CloudDensityCSPath, m_cloudDensityCS);
    FGlobalPSO::Get()->CompileCS(CloudLightingCSPath, m_cloudLightingCS);

    
    FSceneRender::Get()->AddCustomRenderObject(this);
}

void ACloud::InitAllComponents()
{
    Actor::InitAllComponents();
    
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

    {
        //0번슬롯 바인딩
        m_volumeConstsGpu->CSSetConstantBuffer(EConstBufferSlot::CB_MaterialDesc, 1);
        context->CSSetUnorderedAccessViews( 0, 1, densityTex.GetAddressOfUAV(), NULL);
        context->CSSetShader(m_cloudDensityCS.Get(), 0, 0);
        context->Dispatch(UINT(ceil(m_volumeWidth / 16.0f)),
                            UINT(ceil(m_volumeHeight / 16.0f)),
                            UINT(ceil(m_volumeDepth / 4.0f)));
    
        D3D::Get()->ComputeShaderBarrier();
    }
    
    {
        context->CSSetShaderResources(0, 1, densityTex.GetAddressOfSRV());
        context->CSSetUnorderedAccessViews(0, 1, lightingTex.GetAddressOfUAV(), NULL);
        context->CSSetShader(m_cloudLightingCS.Get(), 0, 0);
        context->Dispatch(UINT(ceil(m_lightWidth / 16.0f)),
                            UINT(ceil(m_lightHeight / 16.0f)),
                            UINT(ceil(m_lightDepth / 4.0f)));

        D3D::Get()->ComputeShaderBarrier();
    }

    WorldCBuffer = make_shared<ConstantBuffer>(&meshWorld, sizeof(Matrix));

}

void ACloud::UpdateGUI()
{
}

void ACloud::Tick(float deltaTime)
{
    Actor::Tick(deltaTime);
    
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();
    Offset += 0.3f * deltaTime; // 애니메이션 효과
    {
        m_volumeConstsCpu.lightDir = FSceneView::Get()->GetDirectionalLightDir();
        m_volumeConstsCpu.uvwOffset.z = Offset;
        m_volumeConstsGpu->UpdateConstBuffer();
        m_volumeConstsGpu->CSSetConstantBuffer(EConstBufferSlot::CB_MaterialDesc, 1);
        context->CSSetUnorderedAccessViews(0, 1, densityTex.GetAddressOfUAV(), NULL);
        context->CSSetShader(m_cloudDensityCS.Get(), 0, 0);
        context->Dispatch(UINT(ceil(m_volumeWidth / 16.0f)),
                            UINT(ceil(m_volumeHeight / 16.0f)),
                            UINT(ceil(m_volumeDepth / 4.0f)));
        D3D::Get()->ComputeShaderBarrier();
    }

    
    {
        context->CSSetShaderResources(0, 1, densityTex.GetAddressOfSRV());
        context->CSSetUnorderedAccessViews(0, 1, lightingTex.GetAddressOfUAV(), NULL);
        context->CSSetShader(m_cloudLightingCS.Get(), 0, 0);
        context->Dispatch(UINT(ceil(m_lightWidth / 16.0f)),
                            UINT(ceil(m_lightHeight / 16.0f)),
                            UINT(ceil(m_lightDepth / 4.0f)));
        D3D::Get()->ComputeShaderBarrier();
    }
}

void ACloud::CustomRender(float deltaTime)
{
    Super::CustomRender(deltaTime);

  
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->SimpleTexturePSO);

    VBuffer->IASetVertexBuffer();
    IBuffer->IASetIndexBuffer();

    meshWorld = meshTransform.ToMatrix().Transpose();
    WorldCBuffer->UpdateConstBuffer();
    WorldCBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_World, 1);
    D3D::Get()->GetDeviceContext()->DrawIndexed(meshData.Indices.size(), 0, 0);
    
}
