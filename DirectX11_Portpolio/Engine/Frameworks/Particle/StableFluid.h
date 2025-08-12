#pragma once


struct FFluidsConst
{
    float dt;
    float viscosity;
    uint32_t i;
    uint32_t j;
    Vector2 sourcingVelocity;
    float pad[2];
    Vector4 sourcingDensity;
};

struct FFluidTexture
{
    ComPtr<ID3D11Texture2D> texture = nullptr;
    ComPtr<ID3D11ShaderResourceView> srv = nullptr;
    ComPtr<ID3D11UnorderedAccessView> uav = nullptr;
};


class StableFluid
{
public:
    StableFluid() = default;
    virtual ~StableFluid() = default;
    virtual void Render(float deltaTime);
    virtual void Initialize(const UINT InWidth, const UINT InHeight);

    
    void Sourcing();
    void Diffuse();
    void Projection();
    void Advection();


    FFluidTexture VelocityTex, VelocityTempTex;
    FFluidTexture VorticityTex;
    FFluidTexture PressureTex, PressureTempTex;
    FFluidTexture DivergenceTex;
    FFluidTexture DensityTex, DensityTempTex;
    
    FFluidsConst FluidConstCPU;
    
private:
    ComPtr<ID3D11ComputeShader> AdvectionCS;
    ComPtr<ID3D11ComputeShader> ApplyPressureCS;
    ComPtr<ID3D11ComputeShader> DiffuseCS;
    ComPtr<ID3D11ComputeShader> DivergenceCS;
    ComPtr<ID3D11ComputeShader> JacobiCS;
    ComPtr<ID3D11ComputeShader> SourcingCS;
    ComPtr<ID3D11ComputeShader> ComputeVorticityCS;
    ComPtr<ID3D11ComputeShader> ConfineVorticityCS;

    shared_ptr<ConstantBuffer> FluidConstGPU;

    UINT m_width = 0;
    UINT m_height = 0;

private:
    const wstring HlslPath = L"../../Engine/Render/HLSL/StableFluids/";

    const wstring AdvectionCSPath = HlslPath + L"AdvectionCS.hlsl";
    const wstring ApplyPressureCSPath = HlslPath + L"ApplyPressureCS.hlsl";
    const wstring DiffuseCSPath = HlslPath + L"DiffuseCS.hlsl";
    const wstring DivergenceCSPath = HlslPath + L"DivergenceCS.hlsl";
    const wstring JacobiCSPath = HlslPath + L"JacobiCS.hlsl";
    const wstring SourcingCSPath = HlslPath + L"SourcingCS.hlsl";
    const wstring ComputeVorticityCSPath = HlslPath + L"ComputeVorticityCS.hlsl";
    const wstring ConfineVorticityCSPath = HlslPath + L"ConfineVorticityCS.hlsl";

};
