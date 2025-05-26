#pragma once

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;
    Shader(wstring VSPath, wstring PSPath);
    static void InitDefaultState();
    
    void InitRenderer(const vector<D3D11_INPUT_ELEMENT_DESC>& InInputElements,
         ESamplerSlot SampSlot);
    void Bind();
    void DrawIndexed(const int nIndex);
    

public:
    ID3D11VertexShader* GetVertexShader() const { return VertexShader.Get(); }
    ID3D11PixelShader* GetPixelShader() const { return PixelShader.Get(); }
    ID3D11InputLayout* GetInputLayout() const { return InputLayouts.Get(); }
    
    void VSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const;
    void PSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const;
    void SetVertexShaderPath(const wstring& InVertexShaderPath);
    void SetPixelShaderPath(const wstring& InPixelShaderPath);

public:
    void SetStencilMaskPipeline();
    void SetReflactPipeline();
    void SetMirrorPipeline();

public:
    // Depth Stencil
    static void SetDefaultDepthStencilState();
    void CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& InDepthStencilDesc);
    void SetCustomDepthStencilState();
  
    
    // PixelShader
    void PSSetSampler();

    // SamplerState
    void CreateSamplerState(const ESamplerSlot SampSlot);
    
private:
    static void CreateDefaultDepthStencilState();
    static void CreateDefaultRasterizeState();
    static void CreateDefaultBlendState();
    static void CreateDepthOnlyVS();
    static void CreateDepthOnlyPS();

    void CompileVertexShader();
    void CompilePixelShader();
    void CreateRasterizeState();
    void CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC>& InInputElements);
    void SetBlendFactor(const float InBlendFactor[4]);

private:
    // VertexShader
    ComPtr<ID3D11VertexShader> VertexShader;
    static ComPtr<ID3D11VertexShader> DepthOnlyVS;

    // PixelShader
    ComPtr<ID3D11PixelShader> PixelShader;
    static ComPtr<ID3D11PixelShader> DepthOnlyPS;


    // Rasterize States
    ComPtr<ID3D11RasterizerState> RSState;
    static ComPtr<ID3D11RasterizerState> solidCCWRS;
    static ComPtr<ID3D11RasterizerState> wireRS;
    static ComPtr<ID3D11RasterizerState> wireCCWRS;

    // Sampler States
    ComPtr<ID3D11SamplerState> SamplerState = nullptr;
    ComPtr<ID3D11SamplerState> ClampSamplerState = nullptr;

    // DepthStencil States
    static ComPtr<ID3D11DepthStencilState> DefaultDSS;
    static ComPtr<ID3D11DepthStencilState> MaskDSS;       // 스텐실버퍼에 표시
    static ComPtr<ID3D11DepthStencilState> DrawMaskedDSS; // 스텐실 표시된 곳만
    ComPtr<ID3D11DepthStencilState> CustomDSS;

    // Blend States
    static ComPtr<ID3D11BlendState> MirrorBS;

    // InputLayout
    ComPtr<ID3D11InputLayout> InputLayouts;

    // Blob
    ComPtr<ID3DBlob> VsBlob;
    ComPtr<ID3DBlob> PsBlob;
    ComPtr<ID3DBlob> ErrorBlob;

public:
    wstring VertexShaderPath;
    wstring PixelShaderPath;
    static bool DrawAsWire;
    
private:
    void Assert_IF_FailedCompile(HRESULT hr);
    float BlendFactor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
};
