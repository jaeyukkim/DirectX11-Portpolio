#include "HeaderCollection.h"
#include <fstream>
#include <d3dcompiler.h>
#include "FSceneView.h"
#include "Shader.h"


Shader::Shader()
{
    
    
}

Shader::~Shader()
{
    
}

void Shader::InitRenderer(const vector<D3D11_INPUT_ELEMENT_DESC>& InInputElements, const D3D11_SAMPLER_DESC& InSamplerDesc)
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
    Assert_IF_FailedCompile(hr);

    hr = D3D::Get()->GetDevice()->CreateVertexShader(VsBlob->GetBufferPointer(),
        VsBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf());
    AssertHR(hr, "CreateVertexShader 에서 실패");


}

void Shader::CompilePixelShader()
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(PixelShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS_Main", "ps_5_0", flags, 0, PsBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert_IF_FailedCompile(hr);

    hr = D3D::Get()->GetDevice()->CreatePixelShader(PsBlob->GetBufferPointer(),
        PsBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf());
    AssertHR(hr, "PixelShader Create 실패");
}


void Shader::Assert_IF_FailedCompile(HRESULT hr)
{
    // 컴파일 실패 시 오류 메시지를 출력하고 프로그램 종료
    if (FAILED(hr))
    {
        if (ErrorBlob.Get() != nullptr)
        {
            // 오류 메시지를 가져와서 출력
            string str = (const char*)ErrorBlob.Get()->GetBufferPointer();
            MessageBoxA(nullptr, str.c_str(), "Shader Error", MB_OK);
        }

        // 오류 발생 시 강제 중단
        assert(false && "Fx File not found");
    }
}


void Shader::CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC>& InInputElements)
{
   
    HRESULT hr = D3D::Get()->GetDevice()->CreateInputLayout(&InInputElements[0], UINT(InInputElements.size()),
        VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(), &InputLayouts);

    FAILED(hr);
    
}

void Shader::CreateSamplerState(const D3D11_SAMPLER_DESC& InSamplerDesc)
{
    D3D::Get()->GetDevice()->CreateSamplerState(&InSamplerDesc, SamplerState.GetAddressOf());
}


void Shader::Bind() const
{
    D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayouts.Get());
    D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), nullptr, 0);
    D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), nullptr, 0);
    D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
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

void Shader::SetVertexShaderPath(const wstring& InVertexShaderPath)
{
    VertexShaderPath = InVertexShaderPath;
}

void Shader::SetPixelShaderPath(const wstring& InPixelShaderPath)
{
    PixelShaderPath = InPixelShaderPath;
}

void Shader::DrawIndexed(const int nIndex)
{
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(FSceneView::Get()->GetPrimitiveType());
    D3D::Get()->GetDeviceContext()->DrawIndexed(nIndex, 0, 0);
}
