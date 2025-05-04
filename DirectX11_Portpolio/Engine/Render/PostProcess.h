#pragma once

class ImageFilter;
class Geometry;
class IndexBuffer;
class VertexBuffer;


class PostProcess 
{
public:
    void Initialize(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
            const vector<ComPtr<ID3D11RenderTargetView>>& targets);
    void Render();
    void CreateBuffer(ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv);

public:
    ImageFilter CombineFilter;
    vector<ImageFilter> BloomDownFilters;
    vector<ImageFilter> BloomUpFilters;

private:
    shared_ptr<StaticMeshData> MeshData;
    shared_ptr<VertexBuffer> VBuffer;
    shared_ptr<IndexBuffer> IBuffer;
    UINT IdxCount = 0;


    vector<ComPtr<ID3D11ShaderResourceView>> bloomSRVs;
    vector<ComPtr<ID3D11RenderTargetView>> bloomRTVs;

private:
    const wstring VSPath = L"VS_Sampling.hlsl";
    const wstring BloomDownPSPath = L"PS_BloomDown.hlsl";
    const wstring BloomUpPSPath = L"PS_BloomUp.hlsl";
    const wstring CombinePSPath = L"PS_Combine.hlsl";
    const int BloomLevels = 4;
};
