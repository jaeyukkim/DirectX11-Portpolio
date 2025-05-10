#pragma once

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;
    Shader(wstring VSPath, wstring PSPath);
    
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
    static void CreateDefaultDepthStencilState();
    static void SetDefaultDepthStencilState();
    void CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& InDepthStencilDesc);
    void SetCustomDepthStencilState();
    void PSSetSampler();
    void CreateSamplerState(const ESamplerSlot SampSlot);

private:
    void CompileVertexShader();
    void CompilePixelShader();
    void CreateRasterizeState();
    void CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC>& InInputElements);
    

private:
    ComPtr<ID3D11VertexShader> VertexShader;
    ComPtr<ID3D11PixelShader> PixelShader;
    ComPtr<ID3D11RasterizerState> RSState;
    ComPtr<ID3D11SamplerState> SamplerState = nullptr;
    ComPtr<ID3D11SamplerState> ClampSamplerState = nullptr;
    static ComPtr<ID3D11DepthStencilState> DefaultDepthStencilState;
    ComPtr<ID3D11DepthStencilState> CustomDepthStencilState;
    ComPtr<ID3D11InputLayout> InputLayouts;
    ComPtr<ID3DBlob> VsBlob;
    ComPtr<ID3DBlob> PsBlob;
    ComPtr<ID3DBlob> ErrorBlob;
    


    wstring VertexShaderPath;
    wstring PixelShaderPath;
    
private:
    void Assert_IF_FailedCompile(HRESULT hr);
};
