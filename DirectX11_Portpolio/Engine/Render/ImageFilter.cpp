#include "HeaderCollection.h"
#include "VertexData.h"
#include "ImageFilter.h"

ImageFilter::ImageFilter(int width, int height)
{
    Initialize(width, height);
}

void ImageFilter::Initialize(int width, int height)
{
    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = (float)width;
    m_viewport.Height = (float)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    FilterData.dx = 1.0f / m_viewport.Width;
    FilterData.dy = 1.0f / m_viewport.Height;

    CBuffer = make_shared<ConstantBuffer>(&FilterData, sizeof(FilterData));
   
}

void ImageFilter::UpdateConstantBuffers() 
{
    CBuffer->UpdateConstBuffer();
}

void ImageFilter::DrawIndexed(UINT IndexCount) const
{

    Assert(SRV.size() > 0, "SRV가 없습니다");
    Assert(RTV.size() > 0, "RTV가 없습니다");


    D3D::Get()->GetDeviceContext()->RSSetViewports(1, &m_viewport);
    D3D::Get()->GetDeviceContext()->OMSetRenderTargets(UINT(RTV.size()), RTV.data(), NULL);

    
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, UINT(SRV.size()), SRV.data());
    CBuffer->PSSetConstantBuffer(EConstBufferSlot::ImageFilterData, 1);
    Renderer::Get()->DrawIndexed(IndexCount);
}

void ImageFilter::SetShaderResources(const vector<ComPtr<ID3D11ShaderResourceView>>& resources) 
{
    SRV.clear();

    for (const auto& res : resources) 
    {
        SRV.push_back(res.Get());
    }
}

void ImageFilter::SetRenderTargets(const vector<ComPtr<ID3D11RenderTargetView>>& targets) 
{

    RTV.clear();

    for (const auto& tar : targets) 
    {
        RTV.push_back(tar.Get());
    }
}