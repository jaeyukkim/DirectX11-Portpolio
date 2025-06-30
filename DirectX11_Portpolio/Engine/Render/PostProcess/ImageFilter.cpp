#include <HeaderCollection.h>
#include "ImageFilter.h"


void ImageFilter::Initialize(EPostProcessType type, int width, int height)
{
    PostProcessType = type;
    
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

    ThreadGroupX = width;
    ThreadGroupY = height;
    
}

void ImageFilter::UpdateConstantBuffers() 
{
    CBuffer->UpdateConstBuffer();
}

void ImageFilter::DrawIndexed(UINT IndexCount) const
{

    Assert(SRV.size() > 0, "SRV가 없습니다");
    Assert(RTV.size() > 0, "RTV가 없습니다");

    switch (PostProcessType)
    {
    case EPostProcessType::PPT_Combine:
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->CombinePSO);
        break;
    case EPostProcessType::PPT_BloomDown:
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->BloomDownPSO);
        break;
    case EPostProcessType::PPT_BloomUp:
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->BloomDownPSO);
        break;
    }
    
    D3D::Get()->GetDeviceContext()->RSSetViewports(1, &m_viewport);
    D3D::Get()->GetDeviceContext()->OMSetRenderTargets(UINT(RTV.size()), RTV.data(), NULL);
    
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, UINT(SRV.size()), SRV.data());
    CBuffer->PSSetConstantBuffer(EConstBufferSlot::CB_ImageFilterData, 1);
    FGlobalPSO::Get()->DrawIndexed(IndexCount);
}

void ImageFilter::SetShaderResources(const vector<ComPtr<ID3D11ShaderResourceView>>& resources) 
{
    SRV.clear();

    for (const auto& res : resources) 
    {
        SRV.push_back(res.Get());
    }
}

void ImageFilter::SetUAVResources(const ComPtr<ID3D11UnorderedAccessView>& resources)
{
    UAV = resources;
}

void ImageFilter::SetRenderTargets(const vector<ComPtr<ID3D11RenderTargetView>>& targets) 
{

    RTV.clear();

    for (const auto& tar : targets) 
    {
        RTV.push_back(tar.Get());
    }
}

void ImageFilter::Dispatch()
{
    switch (PostProcessType)
    {
    case EPostProcessType::PPT_GaussianX:
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->XGaussianPSO);
        break;
    case EPostProcessType::PPT_GaussianY:
        FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->YGaussianPSO);
        break;
    }

    D3D::Get()->GetDeviceContext()->CSSetShaderResources(0, 1, SRV.data());
    D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, UAV.GetAddressOf(), NULL);
    D3D::Get()->GetDeviceContext()->Dispatch(ThreadGroupX, ThreadGroupY, 1);
}
