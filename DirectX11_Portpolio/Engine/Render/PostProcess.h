#pragma once

#include "ImageFilter.h"

class Geometry;
class IndexBuffer;
class VertexBuffer;
struct StaticMeshData;

class PostProcess 
{
public:
    PostProcess(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
        const vector<ComPtr<ID3D11RenderTargetView>>& targets);

    void Initialize(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
            const vector<ComPtr<ID3D11RenderTargetView>>& targets);
    void Render();
    void CreateBuffer(ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv, int width, int height);

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
    
    const wstring VSPath = L"../Engine/HLSL/VS_Sampling.hlsl";
    const wstring BloomDownPSPath = L"../Engine/HLSL/PS_BloomDown.hlsl";
    const wstring BloomUpPSPath = L"../Engine/HLSL/PS_BloomUp.hlsl";
    const wstring CombinePSPath = L"../Engine/HLSL/PS_Combine.hlsl";
    const int BloomLevels = 4;
};
