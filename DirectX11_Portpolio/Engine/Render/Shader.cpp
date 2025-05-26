#include "HeaderCollection.h"
#include <fstream>
#include <d3dcompiler.h>
#include "FSceneView.h"
#include "Shader.h"


ComPtr < ID3D11VertexShader> Shader::DepthOnlyVS = nullptr;
ComPtr < ID3D11PixelShader> Shader::DepthOnlyPS = nullptr;

ComPtr<ID3D11DepthStencilState> Shader::DefaultDSS = nullptr;
ComPtr<ID3D11DepthStencilState> Shader::MaskDSS = nullptr;
ComPtr<ID3D11DepthStencilState> Shader::DrawMaskedDSS = nullptr;

ComPtr<ID3D11RasterizerState> Shader::wireRS = nullptr;
ComPtr<ID3D11RasterizerState> Shader::wireCCWRS = nullptr;
ComPtr<ID3D11RasterizerState> Shader::solidCCWRS = nullptr;

ComPtr<ID3D11BlendState> Shader::MirrorBS = nullptr;
bool Shader::DrawAsWire = false;


Shader::Shader(wstring VSPath, wstring PSPath)
{
    SetVertexShaderPath(VSPath);
    SetPixelShaderPath(PSPath);
}

void Shader::InitDefaultState()
{
    CreateDefaultDepthStencilState();
    CreateDefaultRasterizeState();
    CreateDefaultBlendState();
    CreateDepthOnlyVS();
    CreateDepthOnlyPS();
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

void Shader::SetBlendFactor(const float InBlendFactor[4])
{
     memcpy(BlendFactor, InBlendFactor, sizeof(float) * 4);
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

    if(DrawAsWire)
        D3D::Get()->GetDeviceContext()->RSSetState(wireRS.Get());
    else
    {
        D3D::Get()->GetDeviceContext()->RSSetState(RSState.Get());
    }
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

void Shader::SetStencilMaskPipeline()
{
    Bind();
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(MaskDSS.Get(), 1);
    D3D::Get()->GetDeviceContext()->VSSetShader(DepthOnlyVS.Get(), nullptr, 0);
    D3D::Get()->GetDeviceContext()->PSSetShader(DepthOnlyPS.Get(), nullptr, 0);
}

void Shader::SetReflactPipeline()
{
    Bind();
    if (DrawAsWire)
    {
        D3D::Get()->GetDeviceContext()->RSSetState(solidCCWRS.Get());
        D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(MaskDSS.Get(), 1);
    }
    else
    {
        D3D::Get()->GetDeviceContext()->RSSetState(wireCCWRS.Get());
    }
}

void Shader::SetMirrorPipeline()
{
    Bind();
    D3D::Get()->GetDeviceContext()->OMSetBlendState(MirrorBS.Get(), BlendFactor, 0xffffffff);
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(MaskDSS.Get(), 1);
}

void Shader::CreateDefaultDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false; // Stencil 불필요
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    

    if (FAILED(D3D::Get()->GetDevice()->CreateDepthStencilState(
        &dsDesc, DefaultDSS.GetAddressOf())))
    {
        cout << "CreateDepthStencilState() failed." << endl;
    }

    // Stencil에 1로 표기해주는 DSS
    dsDesc.DepthEnable = true; // 이미 그려진 물체 유지
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = true;    // Stencil 필수
    dsDesc.StencilReadMask = 0xFF;  // 모든 비트 다 사용
    dsDesc.StencilWriteMask = 0xFF; // 모든 비트 다 사용
    
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    if (FAILED(D3D::Get()->GetDevice()->CreateDepthStencilState(
        &dsDesc, MaskDSS.GetAddressOf())))
    {
        cout << "maskDSS() failed." << endl;
    }
    
    // Stencil에 1로 표기된 경우에"만" 그리는 DSS
    // DepthBuffer는 초기화된 상태로 가정
    // D3D11_COMPARISON_EQUAL 이미 1로 표기된 경우에만 그리기
    // OMSetDepthStencilState(..., 1); <- 여기의 1
    dsDesc.DepthEnable = true;   // 거울 속을 다시 그릴때 필요
    dsDesc.StencilEnable = true; // Stencil 사용
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // <- 주의
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    if (FAILED(D3D::Get()->GetDevice()->CreateDepthStencilState(
        &dsDesc, DrawMaskedDSS.GetAddressOf())))
    {
        cout << "drawMaskedDSS() failed." << endl;
    }
}

void Shader::SetDefaultDepthStencilState()
{
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(Shader::DefaultDSS.Get(), 0);
}

void Shader::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& InDepthStencilDesc)
{
    if (FAILED(D3D::Get()->GetDevice()->CreateDepthStencilState(
        &InDepthStencilDesc, CustomDSS.GetAddressOf())))
    {
        cout << "CreateDepthStencilState() failed." << endl;
    }
}

void Shader::SetCustomDepthStencilState()
{
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(CustomDSS.Get(), 0);
}

void Shader::CreateDefaultRasterizeState()
{
    // Rasterizer States
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    rastDesc.FrontCounterClockwise = true;

    // 거울에 반사되면 삼각형의 Winding이 바뀌기 때문에 CCW로 그려야함
    if (FAILED(D3D::Get()->GetDevice()->CreateRasterizerState(
        &rastDesc, solidCCWRS.GetAddressOf())))
    {
        cout << "Create solidCCWRS() failed." << endl;
    }
    
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    if (FAILED(D3D::Get()->GetDevice()->CreateRasterizerState(
        &rastDesc, wireCCWRS.GetAddressOf())))
    {
        cout << "Create wireCCWRS() failed." << endl;
    }

    rastDesc.FrontCounterClockwise = false;
    if (FAILED(D3D::Get()->GetDevice()->CreateRasterizerState(
        &rastDesc, wireRS.GetAddressOf())))
    {
        cout << "Create wireRS() failed." << endl;
    }
}

void Shader::CreateDefaultBlendState()
{
    // "이미 그려져있는 화면"과 어떻게 섞을지를 결정
    // Dest: 이미 그려져 있는 값들을 의미
    // Src: 픽셀 쉐이더가 계산한 값들을 의미 (여기서는 마지막 거울)

    D3D11_BLEND_DESC mirrorBlendDesc;
    ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
    mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
    mirrorBlendDesc.IndependentBlendEnable = false;
    
    // 개별 RenderTarget에 대해서 설정 (최대 8개)
    mirrorBlendDesc.RenderTarget[0].BlendEnable = true;
    mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    // 필요하면 RGBA 각각에 대해서도 조절 가능
    mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    if (FAILED(D3D::Get()->GetDevice()->CreateBlendState(
        &mirrorBlendDesc, MirrorBS.GetAddressOf())))
    {
        cout << "Create MirrorBS failed." << endl;
    }
 
}

void Shader::CreateDepthOnlyVS()
{
    ComPtr<ID3DBlob> VsBlob;
    ComPtr<ID3DBlob> ErrorBlob;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    wstring filename = L"../Engine/HLSL/VS_DepthOnly.hlsl";

    HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS_Main", "vs_5_0", flags, 0, VsBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
 
    hr = D3D::Get()->GetDevice()->CreateVertexShader(VsBlob->GetBufferPointer(),
        VsBlob->GetBufferSize(), nullptr, DepthOnlyVS.GetAddressOf());
    AssertHR(hr, "CreateDepthOnlyVS 에서 실패");

}

void Shader::CreateDepthOnlyPS()
{
    ComPtr<ID3DBlob> VsBlob;
    ComPtr<ID3DBlob> ErrorBlob;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    wstring filename = L"../Engine/HLSL/PS_DepthOnly.hlsl";

    HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS_Main", "vs_5_0", flags, 0, VsBlob.GetAddressOf(), ErrorBlob.GetAddressOf());

    hr = D3D::Get()->GetDevice()->CreatePixelShader(VsBlob->GetBufferPointer(),
        VsBlob->GetBufferSize(), nullptr, DepthOnlyPS.GetAddressOf());
    AssertHR(hr, "CreateDepthOnlyPS 에서 실패");
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
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D::Get()->GetDeviceContext()->DrawIndexed(nIndex, 0, 0);
}
