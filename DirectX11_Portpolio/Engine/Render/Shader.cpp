#include "HeaderCollection.h"
#include "Shader.h"
#include <fstream>
#include <d3dcompiler.h>


Shader::Shader()
{
    
    
}

Shader::~Shader()
{
    
}

void Shader::InitRenderer(const vector<D3D11_INPUT_ELEMENT_DESC> &InInputElements, const D3D11_SAMPLER_DESC& InSamplerDesc)
{
    CompileVertexShader();
    CreateInputLayout(InInputElements);
    CompilePixelShader();
    CreateSamplerState(InSamplerDesc);
    this->Bind();
}


void Shader::CompileVertexShader()
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(VertexShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS_Main", "vs_5_0", flags, 0, VsBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert(hr, "VertexShader compile 에서 실패");

    hr = D3D::Get()->GetDevice()->CreateVertexShader(VsBlob->GetBufferPointer(),
        VsBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf());
    Assert(hr, "CreateVertexShader 에서 실패");
}

void Shader::CompilePixelShader()
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(PixelShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS_Main", "ps_5_0", flags, 0, PsBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert(hr, "PixelShader Compile 실패");

    hr = D3D::Get()->GetDevice()->CreatePixelShader(PsBlob->GetBufferPointer(),
        PsBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf());
    Assert(hr, "PixelShader Create 실패");
}

void Shader::CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC> &InInputElements)
{
    
    HRESULT hr = D3D::Get()->GetDevice()->CreateInputLayout(InInputElements.data(), static_cast<UINT>(InInputElements.size()),
                    VsBlob->GetBufferPointer(),VsBlob->GetBufferSize(), InputLayout.GetAddressOf());
    
    Assert(hr, "CreateInputLayout 실패");
}

void Shader::CreateSamplerState(const D3D11_SAMPLER_DESC& InSamplerDesc)
{
    D3D::Get()->GetDevice()->CreateSamplerState(&InSamplerDesc, SamplerState.GetAddressOf());
}

void Shader::Bind() const
{
    D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout.Get());
    D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), nullptr, 0);
    D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), nullptr, 0);
}

void Shader::VSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const
{
    D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(
        StartSlot,
        static_cast<UINT>(InBuffers.size()),
        InBuffers.data());
}

void Shader::PSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const
{
    D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(
        StartSlot,
        static_cast<UINT>(InBuffers.size()),
        InBuffers.data());
}

void Shader::RenderIndexed(const int nIndex, D3D_PRIMITIVE_TOPOLOGY& primitive) const
{
    D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), 0, 0);
   
    D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout.Get());
    
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(primitive);
    D3D::Get()->GetDeviceContext()->DrawIndexed(nIndex, 0, 0);
}
