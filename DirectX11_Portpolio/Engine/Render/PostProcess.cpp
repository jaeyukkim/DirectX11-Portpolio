#include "HeaderCollection.h"
#include "Geometry.h"
#include "RenderData.h"
#include "PostProcess.h"


PostProcess::PostProcess(const vector<ComPtr<ID3D11ShaderResourceView>>& resources, const vector<ComPtr<ID3D11RenderTargetView>>& targets)
{
    Initialize(resources, targets);
}

void PostProcess::Initialize(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
                                            const vector<ComPtr<ID3D11RenderTargetView>>& targets) 
{
    
    MeshData = make_shared<StaticMeshData>(Geometry::MakeSquare());
    VBuffer = make_shared<VertexBuffer>(MeshData->Vertices.data(), MeshData->Vertices.size(), sizeof(VertexObject));
    IdxCount = MeshData->Indices.size();
    IBuffer = make_shared<IndexBuffer>(MeshData->Indices.data(), IdxCount);
    

    int width = D3D::GetDesc().Width;
    int height = D3D::GetDesc().Height;
    bloomSRVs.resize(BloomLevels);
    bloomRTVs.resize(BloomLevels);

    for (int i = 0; i < BloomLevels; i++) 
    {
        int div = int(pow(2, i));
        CreateBuffer(bloomSRVs[i], bloomRTVs[i], width /div, height /div);
    }

    BloomDownFilters.resize(BloomLevels - 1);
    for (int i = 0; i < BloomLevels - 1; i++) 
    {
        int div = int(pow(2, i + 1));
        BloomDownFilters[i].Initialize(VSPath, BloomDownPSPath, width / div, height / div);
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
        BloomUpFilters[i].Initialize(VSPath, BloomUpPSPath,width / div, height / div);
        BloomUpFilters[i].SetShaderResources({ bloomSRVs[level + 1] });
        BloomUpFilters[i].SetRenderTargets({ bloomRTVs[level] });
    }

    // Combine + ToneMapping
    CombineFilter.Initialize(VSPath, CombinePSPath, width, height);
    CombineFilter.SetShaderResources({ resources[0], bloomSRVs[0] });
    CombineFilter.SetRenderTargets(targets);
    CombineFilter.FilterData.strength = 0.0f; // Bloom strength
    CombineFilter.FilterData.option1 = 1.0f;  // Exposure로 사용
    CombineFilter.FilterData.option2 = 2.2f;  // Gamma로 사용
    CombineFilter.UpdateConstantBuffers();
}


void PostProcess::Render() 
{
    
   
    VBuffer->IASetVertexBuffer();
    IBuffer->IASetIndexBuffer();

    for (int i = 0; i < BloomDownFilters.size(); i++) 
    {
        BloomDownFilters[i].DrawIndexed(IdxCount);
    }

    for (int i = 0; i < BloomUpFilters.size(); i++) 
    {
        BloomUpFilters[i].DrawIndexed(IdxCount);
    }

    CombineFilter.DrawIndexed(IdxCount);
}


void PostProcess::CreateBuffer(ComPtr<ID3D11ShaderResourceView>& srv,
                                                   ComPtr<ID3D11RenderTargetView>& rtv, int width, int height)
{
    ComPtr<ID3D11Texture2D> texture;

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //  이미지 처리용도
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = 0;
    txtDesc.CPUAccessFlags = 0;


    D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
    D3D::Get()->GetDevice()->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf());
    D3D::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf());
}