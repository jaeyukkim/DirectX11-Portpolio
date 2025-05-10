#include "HeaderCollection.h"
#include <fstream>
#include <d3dcompiler.h>
#include "FSceneView.h"
#include "Shader.h"

ComPtr<ID3D11DepthStencilState> Shader::DefaultDepthStencilState = nullptr;


Shader::Shader(wstring VSPath, wstring PSPath)
{
    SetVertexShaderPath(VSPath);
    SetPixelShaderPath(PSPath);
}

void Shader::InitRenderer(const vector<D3D11_INPUT_ELEMENT_DESC>& InInputElements, ESamplerSlot SampSlot)
{
    CompileVertexShader();
    CreateInputLayout(InInputElements);
    CompilePixelShader();
    CreateSamplerState(SampSlot);
    CreateRasterizeState();
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

//

void Shader::CreateRasterizeState()
{
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    // rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true; // <- zNear, zFar 확인에 필요
    

    HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&rastDesc, RSState.GetAddressOf());
    AssertHR(hr, "Rasterize Create 실패");
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
   
    HRESULT hr = D3D::Get()->GetDevice()->CreateInputLayout(InInputElements.data(), UINT(InInputElements.size()),
        VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(), &InputLayouts);

    FAILED(hr);
    
}

void Shader::CreateSamplerState(const ESamplerSlot sampSlot)
{
    switch (sampSlot)
    {
    case ESamplerSlot::DefaultSampler:
        D3D::Get()->GetDevice()->CreateSamplerState(&SamplerDescCollection::GetDefaultSamplerDesc(),
            SamplerState.GetAddressOf());
        break;

    case ESamplerSlot::ClampSampler:
        D3D::Get()->GetDevice()->CreateSamplerState(&SamplerDescCollection::GetClampSamplerDesc(),
            ClampSamplerState.GetAddressOf());
        break;
    }
    
}


void Shader::Bind()
{
    D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayouts.Get());
    D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), nullptr, 0);
    D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), nullptr, 0);
    PSSetSampler();
    D3D::Get()->GetDeviceContext()->RSSetState(RSState.Get());
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

void Shader::CreateDefaultDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilDesc.DepthEnable = true; // false
    depthStencilDesc.DepthWriteMask =
        D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc =
        D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
    

    if (FAILED(D3D::Get()->GetDevice()->CreateDepthStencilState(
        &depthStencilDesc, DefaultDepthStencilState.GetAddressOf())))
    {
        cout << "CreateDepthStencilState() failed." << endl;
    }
}

void Shader::SetDefaultDepthStencilState()
{
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(Shader::DefaultDepthStencilState.Get(), 0);
}

void Shader::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& InDepthStencilDesc)
{
    if (FAILED(D3D::Get()->GetDevice()->CreateDepthStencilState(
        &InDepthStencilDesc, CustomDepthStencilState.GetAddressOf())))
    {
        cout << "CreateDepthStencilState() failed." << endl;
    }
}

void Shader::SetCustomDepthStencilState()
{
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(CustomDepthStencilState.Get(), 0);
}

void Shader::PSSetSampler()
{
    if(SamplerState != nullptr)
        D3D::Get()->GetDeviceContext()->PSSetSamplers(static_cast<int>(ESamplerSlot::DefaultSampler), 1, SamplerState.GetAddressOf());
    if(ClampSamplerState != nullptr)
        D3D::Get()->GetDeviceContext()->PSSetSamplers(static_cast<int>(ESamplerSlot::ClampSampler), 1, ClampSamplerState.GetAddressOf());
}

void Shader::DrawIndexed(const int nIndex)
{
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(FSceneView::Get()->GetPrimitiveType());
    D3D::Get()->GetDeviceContext()->DrawIndexed(nIndex, 0, 0);
}
