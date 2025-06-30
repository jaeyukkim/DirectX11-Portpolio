#include <HeaderCollection.h>
#include <Render/Mesh/GeometryGenerator.h>
#include "PostProcess.h"


PostProcess::PostProcess(const vector<ComPtr<ID3D11ShaderResourceView>>& resources, const vector<ComPtr<ID3D11RenderTargetView>>& targets)
{
    Initialize(resources, targets);
}


void PostProcess::Initialize(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
                                            const vector<ComPtr<ID3D11RenderTargetView>>& targets) 
{
    TextureBuffer::CreateBuffer(PostProcess_TEX, PostProcessSRV, PostProcessRTV, D3D::GetDesc().Width, D3D::GetDesc().Height);

    InitializeBloomFilter(resources, targets);
    InitializeCombineFilter(resources, targets);

    InitializeGaussianFilter();
    
}


void PostProcess::InitializeCombineFilter(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
        const vector<ComPtr<ID3D11RenderTargetView>>& targets)
{
    int width = D3D::GetDesc().Width;
    int height = D3D::GetDesc().Height;
    
    // Combine + ToneMapping
    CombineFilter.Initialize(EPostProcessType::PPT_Combine, width, height);
    CombineFilter.SetShaderResources({ resources[0], PostProcessSRV });
    CombineFilter.SetRenderTargets(targets);
    CombineFilter.FilterData.strength = 0.0f; // Bloom strength
    CombineFilter.FilterData.option1 = 1.0f;  // Exposure로 사용
    CombineFilter.FilterData.option2 = 2.2f;  // Gamma로 사용
    CombineFilter.UpdateConstantBuffers();
}


void PostProcess::InitializeBloomFilter(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
    const vector<ComPtr<ID3D11RenderTargetView>>& targets)
{
    MeshData = make_shared<StaticMeshData>(GeometryGenerator::MakeSquare());
    VBuffer = make_shared<VertexBuffer>(MeshData->Vertices.data(), MeshData->Vertices.size(), sizeof(VertexObject));
    IdxCount = MeshData->Indices.size();
    IBuffer = make_shared<IndexBuffer>(MeshData->Indices.data(), IdxCount);
    

    int width = D3D::GetDesc().Width;
    int height = D3D::GetDesc().Height;
    bloomSRVs.resize(BloomLevels);
    bloomRTVs.resize(BloomLevels);
    bloomTexs.resize(BloomLevels);
    
    for (int i = 0; i < BloomLevels; i++) 
    {
        int div = int(pow(2, i));
        TextureBuffer::CreateBuffer(bloomTexs[i], bloomSRVs[i], bloomRTVs[i], width /div, height /div);
    }

    BloomDownFilters.resize(BloomLevels - 1);
    for (int i = 0; i < BloomLevels - 1; i++) 
    {
        int div = int(pow(2, i + 1));
        BloomDownFilters[i].Initialize(EPostProcessType::PPT_BloomDown, width / div, height / div);
        if (i == 0) 
        {
            BloomDownFilters[i].SetShaderResources({ resources[0] });
        }
        else 
        {
            BloomDownFilters[i].SetShaderResources({ bloomSRVs[i] });
        }

        BloomDownFilters[i].SetRenderTargets({ bloomRTVs[i + 1] });
    }

    BloomUpFilters.resize(BloomLevels - 1);
    for (int i = 0; i < BloomLevels - 1; i++) 
    {
        int level = BloomLevels - 2 - i;
        int div = int(pow(2, level));
        BloomUpFilters[i].Initialize(EPostProcessType::PPT_BloomUp, width / div, height / div);
        BloomUpFilters[i].SetShaderResources({ bloomSRVs[level + 1] });
        BloomUpFilters[i].SetRenderTargets({ bloomRTVs[level] });
    }
    
}

void PostProcess::InitializeGaussianFilter()
{
    PrepareStagingTexture();

    int screenWidth = D3D::Get()->GetDesc().Width;
    int screenHeight = D3D::Get()->GetDesc().Height;
    UINT ThreadGroupX = UINT(ceil(screenWidth / 256.0f));
    UINT ThreadGroupY = UINT(ceil(screenHeight / 256.0f));

    GaussianX_Filter.Initialize(EPostProcessType::PPT_GaussianX, ThreadGroupX, screenHeight);
    GaussianY_Filter.Initialize(EPostProcessType::PPT_GaussianY, screenWidth, ThreadGroupY);

    ComPtr<ID3D11RenderTargetView> tempRTV;
    TextureBuffer::CreateUAVTexture(D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height,
        DXGI_FORMAT_R16G16B16A16_FLOAT, GaussianX_TEX, tempRTV, GaussianX_SRV, GaussianX_UAV);

    ComPtr<ID3D11RenderTargetView> tempRTV2;
    TextureBuffer::CreateUAVTexture(D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height,
            DXGI_FORMAT_R16G16B16A16_FLOAT, GaussianY_TEX, tempRTV2, GaussianY_SRV, GaussianY_UAV);

    
    /* BackBuffer에 있는 텍스쳐가 GaussianX_SRV로 저장되어짐 그 후 GaussianY_UAV텍스쳐를 도화지로
     * 삼아 X방향 블러를 수행 */
    GaussianX_Filter.SetShaderResources({GaussianX_SRV});
    GaussianX_Filter.SetUAVResources({GaussianY_UAV});
    GaussianY_Filter.SetShaderResources({GaussianY_SRV});
    GaussianY_Filter.SetUAVResources({GaussianX_UAV});

}



void PostProcess::Render() 
{
    D3D::Get()->GetDeviceContext()->CopyResource(PostProcess_TEX.Get(), D3D::Get()->PostEffectsBuffer.Get());

    VBuffer->IASetVertexBuffer();
    IBuffer->IASetIndexBuffer();
    
    //RenderBloomFilter();
    ComputeGaussianBlur();
    CombineFilter.DrawIndexed(IdxCount);
}

void PostProcess::RenderBloomFilter()
{
    
    for (int i = 0; i < BloomDownFilters.size(); i++) 
    {
        BloomDownFilters[i].DrawIndexed(IdxCount);
    }

    for (int i = 0; i < BloomUpFilters.size(); i++) 
    {
        BloomUpFilters[i].DrawIndexed(IdxCount);
    }

    D3D::Get()->GetDeviceContext()->CopyResource(PostProcess_TEX.Get(), bloomTexs[0].Get());

}

void PostProcess::ComputeGaussianBlur()
{
    //D3D::Get()->GetDeviceContext()->CopyResource(StagingTexture.Get(), PostProcess_TEX.Get());
    D3D::Get()->GetDeviceContext()->CopyResource(GaussianX_TEX.Get(), PostProcess_TEX.Get());

    // Horizontal X-Blur, A to B
    for(int i = 0 ; i<10 ; i++)
    {
        GaussianX_Filter.Dispatch();
        D3D::Get()->ComputeShaderBarrier();
        GaussianY_Filter.Dispatch();
        D3D::Get()->ComputeShaderBarrier();
    }

    
    D3D::Get()->GetDeviceContext()->CopyResource(PostProcess_TEX.Get(), GaussianX_TEX.Get());
}


void PostProcess::PrepareStagingTexture()
{
    int width = D3D::GetDesc().Width;
    int height = D3D::GetDesc().Height;

    vector<Vector4> f32Image(width * height, Vector4(0.0f));
    f32Image[width / 4 + width * height / 2] =
            Vector4(50000.0f, 0.0f, 0.0f, 1.0f);
    f32Image[width / 2 + width * height / 2] =
        Vector4(0.0f, 50000.0f, 0.0f, 1.0f);
    f32Image[width / 4 * 3 + width * height / 2] =
        Vector4(0.0f, 0.0f, 50000.0f, 1.0f);
    f32Image[0] = Vector4(3000.0f);
    f32Image[f32Image.size() - 1] = Vector4(3000.0f);

    
    vector<uint8_t> f16Image(width * height * 8); // 16의 절반이라 8
    uint16_t *f16temp = (uint16_t *)f16Image.data();

    
    float *f32temp = (float *)f32Image.data();
    
    for (int i = 0; i < f32Image.size() * 4; i++) // Vector4의 float 4개를 하나씩
    {
        f16temp[i] = fp16_ieee_from_fp32_value(f32temp[i]);
    }

    // Staging Texture 만들고 CPU -> GPU 복사
    StagingTexture = TextureBuffer::CreateStagingTexture(f16Image,
        width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1);
}
