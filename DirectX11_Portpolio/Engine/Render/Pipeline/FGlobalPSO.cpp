#include "HeaderCollection.h"
#include <fstream>
#include <d3dcompiler.h>



FGlobalPSO* FGlobalPSO::Instance = nullptr;
bool FGlobalPSO::DrawAsWire = false;


FGlobalPSO* FGlobalPSO::Get()
{
    return Instance;
}

void FGlobalPSO::Create()
{
    assert(Instance == nullptr);
    Instance = new FGlobalPSO();

    Instance->InitShaderAndState();
}

void FGlobalPSO::Destroy()
{
    Delete(Instance);
}

void FGlobalPSO::InitShaderAndState()
{
    InitVSAndIL();
    InitHullShader();
    InitDomainShader();
    InitGeometryShader();
    InitPixelShader();
    
    InitRasterizeState();
    InitDepthStencilState();
    InitSamplerState();
    InitBlendState();

    InitPSO();
}

void FGlobalPSO::BindPSO(const FPSO& pso)
{
    D3D::Get()->GetDeviceContext()->VSSetShader(pso.m_vertexShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->PSSetShader(pso.m_pixelShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->HSSetShader(pso.m_hullShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->DSSetShader(pso.m_domainShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->GSSetShader(pso.m_geometryShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->IASetInputLayout(pso.m_inputLayout.Get());
    D3D::Get()->GetDeviceContext()->RSSetState(pso.m_rasterizerState.Get());
    D3D::Get()->GetDeviceContext()->OMSetBlendState(pso.m_blendState.Get(), pso.m_blendFactor.data(),
                               0xffffffff);
    D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(pso.m_depthStencilState.Get(),
                                      pso.m_stencilRef);
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(pso.m_primitiveTopology);
    D3D::Get()->GetDeviceContext()->PSSetSamplers(0, ESamplerSlot::MaxSamplerSlot, Samplers[0].GetAddressOf());

}

void FGlobalPSO:: DrawIndexed(const int nIndex)
{
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D::Get()->GetDeviceContext()->DrawIndexed(nIndex, 0, 0);
}


void FGlobalPSO::VSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const
{
    D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(
        StartSlot,
        static_cast<UINT>(InBuffers.size()),
        InBuffers.data());
}

void FGlobalPSO::PSSetConstantBuffers(UINT StartSlot, const vector<ID3D11Buffer*>& InBuffers) const
{
    D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(
        StartSlot,
        static_cast<UINT>(InBuffers.size()),
        InBuffers.data());
}


//-----------------------------------Initialize----------------------------------------//

void FGlobalPSO::InitVSAndIL()
{
    vector<D3D11_INPUT_ELEMENT_DESC> meshIED =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
 
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
    
    vector<D3D11_INPUT_ELEMENT_DESC> samplingIED =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    vector<D3D11_INPUT_ELEMENT_DESC> postProcessingIED =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    
    CompileVSAndInputLayout(MeshVSPath, MeshVS, meshIED, MeshIL);
    CompileVSAndInputLayout(SkeletalMeshVSPath, SkeletalMeshVS, skeletalMeshIED, SkeletalMeshIL);
    CompileVSAndInputLayout(DepthOnlyVSPath, DepthOnlyVS, meshIED, MeshIL);
    CompileVSAndInputLayout(SkyBoxVSPath, SkyboxVS, meshIED, MeshIL);
    CompileVSAndInputLayout(SamplingVSPath, SamplingVS, samplingIED, SamplingIL);
    //CompileVS(NormalVSPath, NormalVS);
}

void FGlobalPSO::InitPixelShader()
{
    CompilePS(MeshPSPath, MeshPS);
    CompilePS(SkeletalMeshPSPath, SkeletalMeshPS);
    CompilePS(DepthOnlyPSPath, DepthOnlyPS);
    CompilePS(SkyBoxPSPath, SkyboxPS);
    CompilePS(CombinePSPath, CombinePS);
    CompilePS(BloomUpPSPath, BloomUpPS);
    CompilePS(BloomDownPSPath, BloomDownPS);
    CompilePS(PostEffectPSPath, PostEffectsPS);
    //CompilePS(NormalPSPath, NormalPS);
}

void FGlobalPSO::InitHullShader()
{
}

void FGlobalPSO::InitDomainShader()
{
}

void FGlobalPSO::InitGeometryShader()
{
    //CompileGS(NormalGSPath, NormalGS);
}


void FGlobalPSO::InitRasterizeState()
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


void FGlobalPSO::InitDepthStencilState()
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


void FGlobalPSO::InitSamplerState()
{
    
    Samplers->Reset();
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
    HRESULT hr = D3D::Get()->GetDevice()->CreateSamplerState(&LinearSDE, Samplers[ESamplerSlot::LinearWrapSampler].GetAddressOf());
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
    hr = D3D::Get()->GetDevice()->CreateSamplerState(&clampSDE, Samplers[ESamplerSlot::LinearClampSampler].GetAddressOf());
    AssertHR(hr, "Create ClampSamplerState failed");


    D3D11_SAMPLER_DESC shadowPointSDE = clampSDE;
    {
        shadowPointSDE.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        shadowPointSDE.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        shadowPointSDE.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        shadowPointSDE.BorderColor[0] = 1.0f;
        shadowPointSDE.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    }
    hr = D3D::Get()->GetDevice()->CreateSamplerState(&shadowPointSDE, Samplers[ESamplerSlot::shadowPointSampler].GetAddressOf());
    AssertHR(hr, "Create shadowPointSampler failed");
    

    D3D11_SAMPLER_DESC shadowCompareSDE = shadowPointSDE;
    {
        shadowCompareSDE.BorderColor[0] = 100.0f; // 큰 Z값
        shadowCompareSDE.Filter =
            D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        shadowCompareSDE.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    }
    hr = D3D::Get()->GetDevice()->CreateSamplerState(&shadowCompareSDE, Samplers[ESamplerSlot::shadowCompareSampler].GetAddressOf());
    AssertHR(hr, "Create shadowCompareSampler failed");
}



void FGlobalPSO::InitBlendState()
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

void FGlobalPSO::InitPSO()
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
    StencilMaskPSO = MeshSolidPSO;
    StencilMaskPSO.m_depthStencilState = MaskDSS;
    StencilMaskPSO.m_stencilRef = 1;
    StencilMaskPSO.m_vertexShader = DepthOnlyVS;
    StencilMaskPSO.m_pixelShader = DepthOnlyPS;


    // MeshReflectSolidPSO: 반사되면 Winding 반대
    MeshReflectSolidPSO = MeshSolidPSO;
    MeshReflectSolidPSO.m_depthStencilState = DrawMaskedDSS;
    MeshReflectSolidPSO.m_rasterizerState = SolidCCWRS; // 반시계
    MeshReflectSolidPSO.m_stencilRef = 1;


    // MeshReflectWirePSO: 반사되면 Winding 반대
    MeshReflectWirePSO = MeshReflectSolidPSO;
    MeshReflectWirePSO.m_rasterizerState = WireCCWRS; // 반시계
    MeshReflectWirePSO.m_stencilRef = 1;


    // SkeletalMeshReflectSolidPSO: 반사되면 Winding 반대
    SkeletalMeshReflectSolidPSO = MeshReflectSolidPSO;
    SkeletalMeshReflectSolidPSO.m_vertexShader = SkeletalMeshVS;
    SkeletalMeshReflectSolidPSO.m_inputLayout = SkeletalMeshIL;
    SkeletalMeshReflectSolidPSO.m_pixelShader = SkeletalMeshPS;


    // SkeletalMeshReflectWirePSO: 반사되면 Winding 반대
    SkeletalMeshReflectWirePSO = SkeletalMeshReflectSolidPSO;
    SkeletalMeshReflectWirePSO.m_rasterizerState = WireCCWRS; // 반시계
    SkeletalMeshReflectWirePSO.m_stencilRef = 1;


    // mirrorBlendSolidPSO;
    MirrorBlendSolidPSO = MeshSolidPSO;
    MirrorBlendSolidPSO.m_blendState = MirrorBS;
    MirrorBlendSolidPSO.m_depthStencilState = DrawMaskedDSS;
    MirrorBlendSolidPSO.m_stencilRef = 1;


    // mirrorBlendWirePSO;
    MirrorBlendWirePSO = MeshWirePSO;
    MirrorBlendWirePSO.m_blendState = MirrorBS;
    MirrorBlendWirePSO.m_depthStencilState = DrawMaskedDSS;
    MirrorBlendWirePSO.m_stencilRef = 1;


    // skyboxSolidPSO
    SkyboxSolidPSO = MeshSolidPSO;
    SkyboxSolidPSO.m_vertexShader = SkyboxVS;
    SkyboxSolidPSO.m_pixelShader = SkyboxPS;
    

    // skyboxWirePSO
    SkyboxWirePSO = SkyboxSolidPSO;
    SkyboxWirePSO.m_rasterizerState = WireRS;


    // reflectSkyboxSolidPSO
    ReflectSkyboxSolidPSO = SkyboxSolidPSO;
    ReflectSkyboxSolidPSO.m_depthStencilState = DrawMaskedDSS;
    ReflectSkyboxSolidPSO.m_rasterizerState = SolidCCWRS; // 반시계
    ReflectSkyboxSolidPSO.m_stencilRef = 1;


    // reflectSkyboxWirePSO
    ReflectSkyboxWirePSO = ReflectSkyboxSolidPSO;
    ReflectSkyboxWirePSO.m_rasterizerState = WireCCWRS;
    ReflectSkyboxWirePSO.m_stencilRef = 1;


    // normalsPSO
    NormalsPSO = MeshSolidPSO;
    NormalsPSO.m_vertexShader = NormalVS;
    NormalsPSO.m_geometryShader = NormalGS;
    NormalsPSO.m_pixelShader = NormalPS;
    NormalsPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;


    // postEffectsPSO
    PostEffectsPSO.m_vertexShader = SamplingVS;
    PostEffectsPSO.m_pixelShader = PostEffectsPS;
    PostEffectsPSO.m_inputLayout = SamplingIL;
    PostEffectsPSO.m_rasterizerState = postProcessingRS;


    // postProcessingPSO
    PostProcessingPSO.m_vertexShader = SamplingVS;
    PostProcessingPSO.m_pixelShader = DepthOnlyPS; // dummy
    PostProcessingPSO.m_inputLayout = SamplingIL;
    PostProcessingPSO.m_rasterizerState = postProcessingRS;


    // BloomUpPSO
    BloomUpPSO = PostProcessingPSO;
    BloomUpPSO.m_pixelShader = BloomUpPS;


    // BloomDownPSO
    BloomDownPSO = PostProcessingPSO;
    BloomDownPSO.m_pixelShader = BloomDownPS;


    // CombinePSO
    CombinePSO = PostProcessingPSO;
    CombinePSO.m_pixelShader = CombinePS;

    //DepthOnlyPSO
    DepthOnlyPSO = MeshSolidPSO;
    DepthOnlyPSO.m_vertexShader = DepthOnlyVS;
    DepthOnlyPSO.m_pixelShader = DepthOnlyPS;
    

}


void FGlobalPSO::CompileVSAndInputLayout(const wstring& path, ComPtr<ID3D11VertexShader>& InVertexShader,
    const vector<D3D11_INPUT_ELEMENT_DESC> &InIE, ComPtr<ID3D11InputLayout>& InIL)
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

    hr = D3D::Get()->GetDevice()->CreateInputLayout(InIE.data(), UINT(InIE.size()),
        Blob->GetBufferPointer(), Blob->GetBufferSize(), InIL.GetAddressOf());
    AssertHR(hr, "Create InputLayout 실패");
}

void FGlobalPSO::CompilePS(const wstring& path, ComPtr<ID3D11PixelShader>& InPixelShader)
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

void FGlobalPSO::CompileHS(const wstring& path, ComPtr<ID3D11HullShader>& InHullShader)
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

void FGlobalPSO::CompileDS(const wstring& path, ComPtr<ID3D11DomainShader>& InDomainShader)
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

void FGlobalPSO::CompileGS(const wstring& path, ComPtr<ID3D11GeometryShader>& InGeometryShader)
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


void FGlobalPSO::Assert_IF_FailedCompile(HRESULT hr, const ComPtr<ID3DBlob>& errorBlob)
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