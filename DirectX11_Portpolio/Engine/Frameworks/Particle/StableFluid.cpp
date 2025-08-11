#include "HeaderCollection.h"
#include "StableFluid.h"


void StableFluid::Initialize(const UINT InWidth, const UINT InHeight)
{
    m_width = InWidth;
    m_height = InHeight;
    
    FluidConstCPU.dt = 0.0f;
    FluidConstCPU.viscosity = 0.0f;
    FluidConstCPU.sourcingVelocity = Vector2(-0.1f, 0.0f);
    FluidConstCPU.sourcingDensity = Vector4(1.0f);  
    FluidConstCPU.i = -1;
    FluidConstCPU.j = -1;

    FluidConstGPU = make_shared<ConstantBuffer>(&FluidConstCPU, sizeof(FluidConstCPU));

    FGlobalPSO::Get()->CompileCS(AdvectionCSPath, AdvectionCS);
    FGlobalPSO::Get()->CompileCS(ApplyPressureCSPath, ApplyPressureCS);
    FGlobalPSO::Get()->CompileCS(DiffuseCSPath, DiffuseCS);
    FGlobalPSO::Get()->CompileCS(DivergenceCSPath, DivergenceCS);
    FGlobalPSO::Get()->CompileCS(JacobiCSPath, JacobiCS);
    FGlobalPSO::Get()->CompileCS(SourcingCSPath, SourcingCS);
    FGlobalPSO::Get()->CompileCS(ComputeVorticityCSPath, ComputeVorticityCS);
    FGlobalPSO::Get()->CompileCS(ConfineVorticityCSPath, ConfineVorticityCS);

    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16G16_FLOAT,
        VelocityTex.texture, VelocityTex.srv, VelocityTex.uav);
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16G16_FLOAT,
        VelocityTempTex.texture, VelocityTempTex.srv, VelocityTempTex.uav);
    
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16_FLOAT,
           PressureTex.texture, PressureTex.srv, PressureTex.uav);
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16_FLOAT,
        PressureTempTex.texture, PressureTempTex.srv, PressureTempTex.uav);
    
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16_FLOAT,
        DivergenceTex.texture, DivergenceTex.srv, DivergenceTex.uav);
    
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16_FLOAT,
        VorticityTex.texture, VorticityTex.srv, VorticityTex.uav);
    
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16G16B16A16_FLOAT,
        DensityTex.texture, DensityTex.srv, DensityTex.uav);
    TextureBuffer::CreateSRVUAVTexture(m_width, m_height, DXGI_FORMAT_R16G16B16A16_FLOAT,
        DensityTempTex.texture, DensityTempTex.srv, DensityTempTex.uav);
    
}
void StableFluid::Render(float deltaTime)
{
    FluidConstCPU.dt = deltaTime;
    FluidConstGPU->UpdateConstBuffer();
    FluidConstGPU->CSSetConstantBuffer(EConstBufferSlot::CB_MaterialDesc, 1);
    
    Sourcing();
    Diffuse();
    Projection();
    Advection();
}

void StableFluid::Sourcing()
{
    ID3D11UnorderedAccessView *uavs[2] = {VelocityTex.uav.Get(),
                                          DensityTex.uav.Get()};
    
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();
    
    context->CSSetUnorderedAccessViews(0, 2, uavs, NULL);
    context->CSSetShader(SourcingCS.Get(), 0, 0);
    context->Dispatch(UINT(ceil(m_width / 32.0f)), UINT(ceil(m_height / 32.0f)),
                      1);
    D3D::Get()->ComputeShaderBarrier();

    // Vorticity confinemenet
    context->CSSetShaderResources(0, 1, VelocityTex.srv.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, VorticityTex.uav.GetAddressOf(),
                                       NULL);
    context->CSSetShader(ComputeVorticityCS.Get(), 0, 0);
    context->Dispatch(UINT(ceil(m_width / 32.0f)), UINT(ceil(m_height / 32.0f)),
                      1);
    D3D::Get()->ComputeShaderBarrier();

    context->CSSetShaderResources(0, 1, VorticityTex.srv.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, VelocityTex.uav.GetAddressOf(),
                                       NULL);
    context->CSSetShader(ConfineVorticityCS.Get(), 0, 0);
    context->Dispatch(UINT(ceil(m_width / 32.0f)), UINT(ceil(m_height / 32.0f)),
                      1);
    D3D::Get()->ComputeShaderBarrier();
}

void StableFluid::Diffuse()
{
    ID3D11ShaderResourceView *evenSRVs[2] = {VelocityTex.srv.Get(),
                                             DensityTex.srv.Get()};
    ID3D11ShaderResourceView *oddSRVs[2] = {VelocityTempTex.srv.Get(),
                                             DensityTempTex.srv.Get()};
    ID3D11UnorderedAccessView *evenUAVs[2] = {VelocityTempTex.uav.Get(),
                                             DensityTempTex.uav.Get()};
    ID3D11UnorderedAccessView *oddUAVs[2] = {VelocityTex.uav.Get(),
                                             DensityTex.uav.Get()};

    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();
    
    context->CSSetShader(DiffuseCS.Get(), 0, 0);

    for (int i = 0; i < 10; i++)
    {

        if (i % 2 == 0)
        {
            context->CSSetShaderResources(0, 2, evenSRVs);
            context->CSSetUnorderedAccessViews(0, 2, evenUAVs, NULL);
        }
        else
        {
            context->CSSetShaderResources(0, 2, oddSRVs);
            context->CSSetUnorderedAccessViews(0, 2, oddUAVs, NULL);
        }

        context->Dispatch(UINT(ceil(m_width / 32.0f)),
                          UINT(ceil(m_height / 32.0f)), 1);

        D3D::Get()->ComputeShaderBarrier();
    }
}

void StableFluid::Projection()
{
    // Compute divergence

    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

    context->CSSetShaderResources(0, 1, VelocityTex.srv.GetAddressOf());

    ID3D11UnorderedAccessView *uavs[3] = {
        DivergenceTex.uav.Get(), PressureTex.uav.Get(), PressureTempTex.uav.Get()};

    context->CSSetUnorderedAccessViews(0, 3, uavs, NULL);
    context->CSSetShader(DivergenceCS.Get(), 0, 0);
    context->Dispatch(UINT(ceil(m_width / 32.0f)), UINT(ceil(m_height / 32.0f)),
                      1);
    D3D::Get()->ComputeShaderBarrier();

    // Jacobi iteration

    context->CSSetShader(JacobiCS.Get(), 0, 0);

    for (int i = 0; i < 100; i++)
    {
        if (i % 2 == 0)
        {
            context->CSSetShaderResources(0, 1, PressureTex.srv.GetAddressOf());
            context->CSSetUnorderedAccessViews(
                0, 1, PressureTempTex.uav.GetAddressOf(), NULL);
        }
        else
        {
            context->CSSetShaderResources(0, 1,
                                          PressureTempTex.srv.GetAddressOf());
            context->CSSetUnorderedAccessViews(
                0, 1, PressureTex.uav.GetAddressOf(), NULL);
        }
        context->CSSetShaderResources(1, 1, DivergenceTex.srv.GetAddressOf());
        context->Dispatch(UINT(ceil(m_width / 32.0f)),
                          UINT(ceil(m_height / 32.0f)), 1);
        D3D::Get()->ComputeShaderBarrier();
    }

    // Apply pressure
    context->CSSetShaderResources(0, 1, PressureTex.srv.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, VelocityTex.uav.GetAddressOf(),
                                       NULL);
    context->CSSetShader(ApplyPressureCS.Get(), 0, 0);
    context->Dispatch(UINT(ceil(m_width / 32.0f)), UINT(ceil(m_height / 32.0f)), 1);
    D3D::Get()->ComputeShaderBarrier();
}

void StableFluid::Advection()
{
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();
    
    context->CopyResource(VelocityTempTex.texture.Get(), VelocityTex.texture.Get());
    context->CopyResource(DensityTempTex.texture.Get(), DensityTex.texture.Get());

    ID3D11ShaderResourceView *srvs[2] = {VelocityTempTex.srv.Get(),
                                         DensityTempTex.srv.Get()};
    ID3D11UnorderedAccessView *uavs[2] = {VelocityTex.uav.Get(),
                                         DensityTex.uav.Get()};

    context->CSSetShaderResources(0, 2, srvs);
    context->CSSetUnorderedAccessViews(0, 2, uavs, NULL);

    context->CSSetShader(AdvectionCS.Get(), 0, 0);
    context->Dispatch(UINT(ceil(m_width / 32.0f)), UINT(ceil(m_height / 32.0f)),
                      1);
    D3D::Get()->ComputeShaderBarrier();
}
