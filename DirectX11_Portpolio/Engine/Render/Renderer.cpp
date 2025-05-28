#include "HeaderCollection.h"
#include <fstream>
#include <d3dcompiler.h>
#include "FSceneView.h"

Renderer* Renderer::Instance = nullptr;
bool Renderer::DrawAsWire = false;


Renderer* Renderer::Get()
{
    return Instance;
}

void Renderer::Create()
{
    assert(Instance == nullptr);
    Instance = new Renderer();

    Instance->InitRenderer();
}

void Renderer::Destroy()
{
    Delete(Instance);
}

void Renderer::InitRenderer()
{
    InitVertexShader();
    InitHullShader();
    InitDomainShader();
    InitGeometryShader();
    InitPixelShader();
    
    InitInputLayout();
    InitRasterizeState();
    InitDepthStencilState();
    InitSamplerState();
    InitBlendState();
}

void Renderer::BindPSO(const FPSO& pso)
{
    D3D::Get()->GetDeviceContext()->VSSetShader(pso.m_vertexShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->PSSetShader(pso.m_pixelShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->HSSetShader(pso.m_hullShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->DSSetShader(pso.m_domainShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->GSSetShader(pso.m_geometryShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->IASetInputLayout(pso.m_inputLayout.Get());
    D3D::Get()->GetDeviceContext()->RSSetState(pso.m_rasterizerState.Get());
    D3D::Get()->GetDeviceContext()->OMSetBlendState(pso.m_blendState.Get(), pso.m_blendFactor,
                               0xffffffff);
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(pso.m_depthStencilState.Get(),
                                      pso.m_stencilRef);
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(pso.m_primitiveTopology);
}

void Renderer::DrawIndexed(const int nIndex)
{
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D::Get()->GetDeviceContext()->DrawIndexed(nIndex, 0, 0);
}


void Renderer::VSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const
{
    D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(
        StartSlot,
        static_cast<UINT>(InBuffers.size()),
        InBuffers.data());
}

void Renderer::PSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const
{
    D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(
        StartSlot,
        static_cast<UINT>(InBuffers.size()),
        InBuffers.data());
}


//-----------------------------------Initialize----------------------------------------//

void Renderer::InitVertexShader()
{
    CompileVS(MeshVSPath, MeshVS);
    CompileVS(SkeletalMeshVSPath, SkeletalMeshVS);
    CompileVS(DepthOnlyVSPath, DepthOnlyVS);
    CompileVS(SkyBoxVSPath, SkyboxVS);
    CompileVS(SamplingVSPath, SamplingVS);
    //CompileVS(NormalVSPath, NormalVS);
}

void Renderer::InitPixelShader()
{
    CompilePS(MeshPSPath, MeshPS);
    CompilePS(SkeletalMeshPSPath, SkeletalMeshPS);
    CompilePS(DepthOnlyPSPath, DepthOnlyPS);
    CompilePS(SkyBoxPSPath, SkyboxPS);
    CompilePS(CombinePSPath, CombinePS);
    CompilePS(BloomUpPSPath, BloomUpPS);
    CompilePS(BloomDownPSPath, BloomDownPS);
    //CompilePS(PostEffectPSPath, PostEffectsPS);
    //CompilePS(NormalPSPath, NormalPS);
}

void Renderer::InitHullShader()
{
}

void Renderer::InitDomainShader()
{
}

void Renderer::InitGeometryShader()
{
    //CompileGS(NormalGSPath, NormalGS);
}



void Renderer::InitInputLayout()
{
    ComPtr<ID3DBlob> Blob;
    ComPtr<ID3DBlob> ErrorBlob;

    vector<D3D11_INPUT_ELEMENT_DESC> meshIED =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    HRESULT hr = D3D::Get()->GetDevice()->CreateInputLayout(meshIED.data(), UINT(meshIED.size()),
        Blob->GetBufferPointer(), ErrorBlob->GetBufferSize(), MeshIL.GetAddressOf());
    AssertHR(hr, "meshIED Create 실패");
    

    vector<D3D11_INPUT_ELEMENT_DESC> skeletalMeshIED =
    {
        { "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
        { "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 8 bytes
        { "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
        { "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
        { "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
        { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 60,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
        { "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 76,  D3D11_INPUT_PER_VERTEX_DATA, 0 }   // 16 bytes
    };
    hr = D3D::Get()->GetDevice()->CreateInputLayout(skeletalMeshIED.data(), UINT(skeletalMeshIED.size()),
        Blob->GetBufferPointer(), ErrorBlob->GetBufferSize(), SkeletalMeshIL.GetAddressOf());
    AssertHR(hr, "skeletalMeshIED Create 실패");

    
    vector<D3D11_INPUT_ELEMENT_DESC> samplingIED =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    hr = D3D::Get()->GetDevice()->CreateInputLayout(samplingIED.data(), UINT(samplingIED.size()),
        Blob->GetBufferPointer(), ErrorBlob->GetBufferSize(), SamplingIL.GetAddressOf());
    AssertHR(hr, "samplingIED Create 실패");

    
    vector<D3D11_INPUT_ELEMENT_DESC> postProcessingIED =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    hr = D3D::Get()->GetDevice()->CreateInputLayout(postProcessingIED.data(), UINT(postProcessingIED.size()),
        Blob->GetBufferPointer(), ErrorBlob->GetBufferSize(), PostProcessingIL.GetAddressOf());
    AssertHR(hr, "postProcessingIED Create 실패");
    
}

void Renderer::InitRasterizeState()
{
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    rastDesc.FrontCounterClockwise = false;

    // solidRS : 기본 Rasterizer
    HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&rastDesc, SolidRS.GetAddressOf());
    AssertHR(hr, "Rasterize Create 실패");
    
    // solidCCWRS : 거울에 반사되면 삼각형의 Winding이 바뀌기 때문에 CCW로 그려야함
    rastDesc.FrontCounterClockwise = true;
    hr = D3D::Get()->GetDevice()->CreateRasterizerState(&rastDesc, SolidCCWRS.GetAddressOf());
    AssertHR(hr, "Create solidCCWRS() failed");
    
    // wireCCWRS : CCW And Wire Rasterizer
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    hr = D3D::Get()->GetDevice()->CreateRasterizerState(&rastDesc, WireCCWRS.GetAddressOf());
    AssertHR(hr, "Create wireCCWRS() failed");
        
    // wireRS : Wire Rasterizer
    rastDesc.FrontCounterClockwise = false;
    hr = D3D::Get()->GetDevice()->CreateRasterizerState(&rastDesc, WireRS.GetAddressOf());
    AssertHR(hr, "Create wireRS() failed");
    
}


void Renderer::InitDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    {
        ZeroMemory(&dsDesc, sizeof(dsDesc));
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = false; // Stencil 불필요
        dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    }
    HRESULT hr = D3D::Get()->GetDevice()->CreateDepthStencilState(&dsDesc, DefaultDSS.GetAddressOf());
    AssertHR(hr, "Create DefaultDSS failed");

    
    // Stencil에 1로 표기해주는 DSS
    {
        dsDesc.DepthEnable = true; // 이미 그려진 물체 유지
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = true;    // Stencil 필수
        dsDesc.StencilReadMask = 0xFF;  
        dsDesc.StencilWriteMask = 0xFF; 
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    }
    hr = D3D::Get()->GetDevice()->CreateDepthStencilState(&dsDesc, MaskDSS.GetAddressOf());
    AssertHR(hr, "Create MaskDSS failed");
    
    
    // Stencil에 1로 표기된 경우에"만" 그리는 DSS
    {
        dsDesc.DepthEnable = true;   // 거울 속을 다시 그릴때 필요
        dsDesc.StencilEnable = true; // Stencil 사용
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // <- 주의
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    }
    hr = D3D::Get()->GetDevice()->CreateDepthStencilState(&dsDesc, DrawMaskedDSS.GetAddressOf());
    AssertHR(hr, "Create drawMaskedDSS failed");
}


void Renderer::InitSamplerState()
{
    
    D3D11_SAMPLER_DESC LinearSDE;
    {
        ZeroMemory(&LinearSDE, sizeof(LinearSDE));
        LinearSDE.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        LinearSDE.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        LinearSDE.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        LinearSDE.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        LinearSDE.ComparisonFunc = D3D11_COMPARISON_NEVER;
        LinearSDE.MinLOD = 0;
        LinearSDE.MaxLOD = D3D11_FLOAT32_MAX;
    }
    HRESULT hr = D3D::Get()->GetDevice()->CreateSamplerState(&LinearSDE, LinearSamplerState.GetAddressOf());
    AssertHR(hr, "Create LinearSamplerState failed");

    
    D3D11_SAMPLER_DESC clampSDE;
    {
        ZeroMemory(&clampSDE, sizeof(clampSDE));
        clampSDE.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        clampSDE.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        clampSDE.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        clampSDE.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        clampSDE.ComparisonFunc = D3D11_COMPARISON_NEVER;
        clampSDE.MinLOD = 0;
        clampSDE.MaxLOD = D3D11_FLOAT32_MAX;
    }
    hr = D3D::Get()->GetDevice()->CreateSamplerState(&clampSDE, ClampSamplerState.GetAddressOf());
    AssertHR(hr, "Create ClampSamplerState failed");
}


void Renderer::InitBlendState()
{
    // Dest: 이미 그려져 있는 값들을 의미
    // Src: 픽셀 쉐이더가 계산한 값들을 의미
    D3D11_BLEND_DESC mirrorBlendDesc;
    {
        ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
        mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
        mirrorBlendDesc.IndependentBlendEnable = false;
        mirrorBlendDesc.RenderTarget[0].BlendEnable = true;
        mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
        mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
        mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask =
            D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    HRESULT hr = D3D::Get()->GetDevice()->CreateBlendState(&mirrorBlendDesc, MirrorBS.GetAddressOf());
    AssertHR(hr, "Create MirrorBS failed");
 
}

void Renderer::InitPSO()
{
    // MeshSolidPSO
    MeshSolidPSO.m_vertexShader = MeshVS;
    MeshSolidPSO.m_inputLayout = MeshIL;
    MeshSolidPSO.m_pixelShader = MeshPS;
    MeshSolidPSO.m_rasterizerState = SolidRS;
    MeshSolidPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // MeshWirePSO
    MeshWirePSO = MeshSolidPSO;
    MeshWirePSO.m_rasterizerState = WireRS;

    // SkeletalMeshSolidPSO
    SkeletalMeshSolidPSO = MeshSolidPSO;
    SkeletalMeshSolidPSO.m_vertexShader = SkeletalMeshVS;
    SkeletalMeshSolidPSO.m_inputLayout = SkeletalMeshIL;
    SkeletalMeshSolidPSO.m_pixelShader = SkeletalMeshPS;
    
    // SkeletalMeshWirePSO
    SkeletalMeshWirePSO = SkeletalMeshSolidPSO;
    SkeletalMeshWirePSO.m_rasterizerState = WireRS;
    
    // stencilMarkPSO;
    stencilMaskPSO = MeshSolidPSO;
    stencilMaskPSO.m_depthStencilState = MaskDSS;
    stencilMaskPSO.m_stencilRef = 1;
    stencilMaskPSO.m_vertexShader = DepthOnlyVS;
    stencilMaskPSO.m_pixelShader = DepthOnlyPS;

    // reflectSolidPSO: 반사되면 Winding 반대
    reflectSolidPSO = MeshSolidPSO;
    reflectSolidPSO.m_depthStencilState = DrawMaskedDSS;
    reflectSolidPSO.m_rasterizerState = SolidCCWRS; // 반시계
    reflectSolidPSO.m_stencilRef = 1;

    // reflectWirePSO: 반사되면 Winding 반대
    reflectWirePSO = reflectSolidPSO;
    reflectWirePSO.m_rasterizerState = WireCCWRS; // 반시계
    reflectWirePSO.m_stencilRef = 1;

    // mirrorBlendSolidPSO;
    mirrorBlendSolidPSO = MeshSolidPSO;
    mirrorBlendSolidPSO.m_blendState = MirrorBS;
    mirrorBlendSolidPSO.m_depthStencilState = DrawMaskedDSS;
    mirrorBlendSolidPSO.m_stencilRef = 1;

    // mirrorBlendWirePSO;
    mirrorBlendWirePSO = MeshWirePSO;
    mirrorBlendWirePSO.m_blendState = MirrorBS;
    mirrorBlendWirePSO.m_depthStencilState = DrawMaskedDSS;
    mirrorBlendWirePSO.m_stencilRef = 1;

    // skyboxSolidPSO
    skyboxSolidPSO = MeshSolidPSO;
    skyboxSolidPSO.m_vertexShader = SkyboxVS;
    skyboxSolidPSO.m_pixelShader = SkyboxPS;
    
    // skyboxWirePSO
    skyboxWirePSO = skyboxSolidPSO;
    skyboxWirePSO.m_rasterizerState = WireRS;

    // reflectSkyboxSolidPSO
    reflectSkyboxSolidPSO = skyboxSolidPSO;
    reflectSkyboxSolidPSO.m_depthStencilState = DrawMaskedDSS;
    reflectSkyboxSolidPSO.m_rasterizerState = SolidCCWRS; // 반시계
    reflectSkyboxSolidPSO.m_stencilRef = 1;

    // reflectSkyboxWirePSO
    reflectSkyboxWirePSO = reflectSkyboxSolidPSO;
    reflectSkyboxWirePSO.m_rasterizerState = WireCCWRS;
    reflectSkyboxWirePSO.m_stencilRef = 1;

    // normalsPSO
    normalsPSO = MeshSolidPSO;
    normalsPSO.m_vertexShader = NormalVS;
    normalsPSO.m_geometryShader = NormalGS;
    normalsPSO.m_pixelShader = NormalPS;
    normalsPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

    // postEffectsPSO
    postEffectsPSO.m_vertexShader = SamplingVS;
    postEffectsPSO.m_pixelShader = PostEffectsPS;
    postEffectsPSO.m_inputLayout = SamplingIL;
    postEffectsPSO.m_rasterizerState = postProcessingRS;

    // postProcessingPSO
    postProcessingPSO.m_vertexShader = SamplingVS;
    postProcessingPSO.m_pixelShader = DepthOnlyPS; // dummy
    postProcessingPSO.m_inputLayout = SamplingIL;
    postProcessingPSO.m_rasterizerState = postProcessingRS;
}


void Renderer::CompileVS(const wstring& path, ComPtr<ID3D11VertexShader>& InVertexShader)
{
    ComPtr<ID3DBlob> Blob;
    ComPtr<ID3DBlob> ErrorBlob;
    
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS_Main", "vs_5_0", flags, 0, Blob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert_IF_FailedCompile(hr, ErrorBlob);

    hr = D3D::Get()->GetDevice()->CreateVertexShader(Blob->GetBufferPointer(),
        Blob->GetBufferSize(), nullptr, InVertexShader.GetAddressOf());
    AssertHR(hr, "CreateVertexShader 에서 실패");
}

void Renderer::CompilePS(const wstring& path, ComPtr<ID3D11PixelShader>& InPixelShader)
{
    ComPtr<ID3DBlob> Blob;
    ComPtr<ID3DBlob> ErrorBlob;
    
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS_Main", "ps_5_0", flags, 0, Blob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert_IF_FailedCompile(hr, ErrorBlob);

    hr = D3D::Get()->GetDevice()->CreatePixelShader(Blob->GetBufferPointer(),
        Blob->GetBufferSize(), nullptr, InPixelShader.GetAddressOf());
    AssertHR(hr, "PixelShader Create 실패");
}

void Renderer::CompileHS(const wstring& path, ComPtr<ID3D11HullShader>& InHullShader)
{
    ComPtr<ID3DBlob> Blob;
    ComPtr<ID3DBlob> ErrorBlob;
    
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "HS_Main", "hs_5_0", flags, 0, Blob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert_IF_FailedCompile(hr, ErrorBlob);

    hr = D3D::Get()->GetDevice()->CreateHullShader(Blob->GetBufferPointer(),
        Blob->GetBufferSize(), nullptr, InHullShader.GetAddressOf());
    AssertHR(hr, "HullShader Create 실패");
}

void Renderer::CompileDS(const wstring& path, ComPtr<ID3D11DomainShader>& InDomainShader)
{
    ComPtr<ID3DBlob> Blob;
    ComPtr<ID3DBlob> ErrorBlob;
    
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "DS_Main", "ds_5_0", flags, 0, Blob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert_IF_FailedCompile(hr, ErrorBlob);

    hr = D3D::Get()->GetDevice()->CreateDomainShader(Blob->GetBufferPointer(),
        Blob->GetBufferSize(), nullptr, InDomainShader.GetAddressOf());
    AssertHR(hr, "DomainShader Create 실패");
}

void Renderer::CompileGS(const wstring& path, ComPtr<ID3D11GeometryShader>& InGeometryShader)
{
    ComPtr<ID3DBlob> Blob;
    ComPtr<ID3DBlob> ErrorBlob;
    
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "GS_Main", "gs_5_0", flags, 0, Blob.GetAddressOf(), ErrorBlob.GetAddressOf());
    Assert_IF_FailedCompile(hr, ErrorBlob);

    hr = D3D::Get()->GetDevice()->CreateGeometryShader(Blob->GetBufferPointer(),
        Blob->GetBufferSize(), nullptr, InGeometryShader.GetAddressOf());
    AssertHR(hr, "GeometryShader Create 실패");
}


void Renderer::Assert_IF_FailedCompile(HRESULT hr, const ComPtr<ID3DBlob>& errorBlob)
{
    // 컴파일 실패 시 오류 메시지를 출력하고 프로그램 종료
    if (FAILED(hr))
    {
        if (errorBlob.Get() != nullptr)
        {
            // 오류 메시지를 가져와서 출력
            string str = (const char*)errorBlob.Get()->GetBufferPointer();
            MessageBoxA(nullptr, str.c_str(), "Shader Error", MB_OK);
        }

        // 오류 발생 시 강제 중단
        assert(false && "hlsl File not found");
    }
}