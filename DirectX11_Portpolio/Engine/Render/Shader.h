#pragma once

class Shader
{
public:
    Shader();
    ~Shader();
    
    void InitRenderer(const vector<D3D11_INPUT_ELEMENT_DESC> &InInputElements, const D3D11_SAMPLER_DESC& InSamplerDesc);
    void Bind() const;
    void RenderIndexed(const int nIndex, D3D_PRIMITIVE_TOPOLOGY& primitive) const;
    

public:
    ID3D11VertexShader* GetVertexShader() const { return VertexShader.Get(); }
    ID3D11PixelShader* GetPixelShader() const { return PixelShader.Get(); }
    ID3D11InputLayout* GetInputLayout() const { return InputLayout.Get(); }
    
    void VSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const;
    void PSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const;
    void SetVertexShaderPath(const wstring& InVertexShaderPath) {PixelShaderPath = InVertexShaderPath;}
    void SetPixelShaderPath(const wstring& InPixelShaderPath) {PixelShaderPath = InPixelShaderPath;}

private:
    void CompileVertexShader();
    void CompilePixelShader();
    void CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC> &InInputElements);
    void CreateSamplerState(const D3D11_SAMPLER_DESC &InSamplerDesc);

private:
    ComPtr<ID3D11VertexShader> VertexShader;
    ComPtr<ID3D11PixelShader> PixelShader;
    ComPtr<ID3D11SamplerState> SamplerState;
    ComPtr<ID3D11InputLayout> InputLayout;
    ComPtr<ID3DBlob> VsBlob;
    ComPtr<ID3DBlob> PsBlob;
    ComPtr<ID3DBlob> ErrorBlob;

    wstring VertexShaderPath;
    wstring PixelShaderPath;
    
};
