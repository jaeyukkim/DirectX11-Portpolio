#pragma once

class Renderer
{
private:
    static Renderer* Instance;
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    
public:
    static Renderer* Get();
    static void Create();
    static void Destroy();

    
public:
    void BindPSO(const FPSO &pso);
    void DrawIndexed(const int nIndex);
    

public:
    ID3D11VertexShader* GetVertexShader() const { return RenderPSO.m_vertexShader.Get(); }
    ID3D11PixelShader* GetPixelShader() const { return RenderPSO.m_pixelShader.Get(); }
    ID3D11InputLayout* GetInputLayout() const { return RenderPSO.m_inputLayout.Get(); }
    
    void VSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const;
    void PSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const;

private:
    void InitRenderer();
    void InitVertexShader();
    void InitPixelShader();
    void InitHullShader();
    void InitDomainShader();
    void InitGeometryShader();
    
    void InitInputLayout();
    void InitRasterizeState();
    void InitDepthStencilState();
    void InitSamplerState();
    void InitBlendState();
    void InitPSO();
    
    void CompileVS(const wstring& path, ComPtr<ID3D11VertexShader>& InVertexShader);
    void CompilePS(const wstring& path, ComPtr<ID3D11PixelShader>& InPixelShader);
    void CompileHS(const wstring& path, ComPtr<ID3D11HullShader>& InHullShader);
    void CompileDS(const wstring& path, ComPtr<ID3D11DomainShader>& InDomainShader);
    void CompileGS(const wstring& path, ComPtr<ID3D11GeometryShader>& InGeometryShader);
    void Assert_IF_FailedCompile(HRESULT hr, const ComPtr<ID3DBlob>& errorBlob);

public:
    static bool DrawAsWire;

    
public:
    FPSO RenderPSO; //실제로 Render 될 PipelineObject
    
    FPSO MeshSolidPSO;
    FPSO SkeletalMeshSolidPSO;
    FPSO MeshWirePSO;
    FPSO SkeletalMeshWirePSO;
    FPSO stencilMaskPSO;
    FPSO reflectSolidPSO;
    FPSO reflectWirePSO;
    FPSO mirrorBlendSolidPSO;
    FPSO mirrorBlendWirePSO;
    FPSO skyboxSolidPSO;
    FPSO skyboxWirePSO;
    FPSO reflectSkyboxSolidPSO;
    FPSO reflectSkyboxWirePSO;
    FPSO normalsPSO;
    FPSO postEffectsPSO;
    FPSO postProcessingPSO;

private:
    // VertexShader
    ComPtr<ID3D11VertexShader> MeshVS;
    ComPtr<ID3D11VertexShader> SkeletalMeshVS;
    ComPtr<ID3D11VertexShader> DepthOnlyVS;
    ComPtr<ID3D11VertexShader> NormalVS;
    ComPtr<ID3D11VertexShader> SkyboxVS;
    ComPtr<ID3D11VertexShader> SamplingVS;
    
    // PixelShader
    ComPtr<ID3D11PixelShader> MeshPS;
    ComPtr<ID3D11PixelShader> SkeletalMeshPS;
    ComPtr<ID3D11PixelShader> SkyboxPS;
    ComPtr<ID3D11PixelShader> NormalPS;
    ComPtr<ID3D11PixelShader> CombinePS;
    ComPtr<ID3D11PixelShader> BloomUpPS;
    ComPtr<ID3D11PixelShader> BloomDownPS;
    ComPtr<ID3D11PixelShader> DepthOnlyPS;
    ComPtr<ID3D11PixelShader> PostEffectsPS;

    // GeometryShader
    ComPtr<ID3D11GeometryShader> NormalGS;
    
    // Rasterize States
    ComPtr<ID3D11RasterizerState> SolidRS;
    ComPtr<ID3D11RasterizerState> SolidCCWRS;
    ComPtr<ID3D11RasterizerState> WireRS;
    ComPtr<ID3D11RasterizerState> WireCCWRS;
    ComPtr<ID3D11RasterizerState> postProcessingRS;
    
    // Sampler States
    ComPtr<ID3D11SamplerState> LinearSamplerState;
    ComPtr<ID3D11SamplerState> ClampSamplerState;

    // DepthStencil States
    ComPtr<ID3D11DepthStencilState> DefaultDSS;
    ComPtr<ID3D11DepthStencilState> MaskDSS;       // 스텐실버퍼에 표시
    ComPtr<ID3D11DepthStencilState> DrawMaskedDSS; // 스텐실 표시된 곳만
    
    // Blend States
    ComPtr<ID3D11BlendState> MirrorBS;

    // InputLayout
    ComPtr<ID3D11InputLayout> MeshIL;
    ComPtr<ID3D11InputLayout> SkeletalMeshIL;
    ComPtr<ID3D11InputLayout> SamplingIL;
    ComPtr<ID3D11InputLayout> PostProcessingIL;

private:
    const wstring HlslPath = L"../Engine/HLSL/";
    
    const wstring MeshVSPath = HlslPath + L"VS_StaticMesh.hlsl";
    const wstring MeshPSPath = HlslPath + L"PS_StaticMesh.hlsl";
    
    const wstring SkeletalMeshVSPath = HlslPath + L"VS_SkeletalMesh.hlsl";
    const wstring SkeletalMeshPSPath = HlslPath + L"PS_SkeletalMesh.hlsl";
    
    const wstring SkyBoxVSPath = HlslPath + L"VS_CubeMap.hlsl";
    const wstring SkyBoxPSPath = HlslPath + L"PS_CubeMap.hlsl";

    const wstring NormalVSPath = HlslPath + L"VS_Normal.hlsl";
    const wstring NormalGSPath = HlslPath + L"GS_Normal.hlsl";
    const wstring NormalPSPath = HlslPath + L"PS_Normal.hlsl";

    const wstring DepthOnlyVSPath = HlslPath + L"VS_DepthOnly.hlsl";
    const wstring DepthOnlyPSPath = HlslPath + L"PS_DepthOnly.hlsl";

    const wstring PostEffectPSPath = HlslPath + L"PS_PostEffect.hlsl";
    
    const wstring SamplingVSPath = HlslPath + L"VS_Sampling.hlsl";
    const wstring SamplingPSPath = HlslPath + L"PS_Sampling.hlsl";

    const wstring BloomUpPSPath = HlslPath + L"PS_BloomUp.hlsl";
    const wstring BloomDownPSPath = HlslPath + L"PS_BloomDown.hlsl";
    
    const wstring CombinePSPath = HlslPath + L"PS_Combine.hlsl";
    
};
