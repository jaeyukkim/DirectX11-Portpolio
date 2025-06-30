#pragma once

#include "ImageFilter.h"

class GeometryGenerator;
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
    void RenderBloomFilter();
    void ComputeGaussianBlur();
   
    
    void PrepareStagingTexture();
public:
    ImageFilter CombineFilter;
    vector<ImageFilter> BloomDownFilters;
    vector<ImageFilter> BloomUpFilters;
    ImageFilter GaussianX_Filter;
    ImageFilter GaussianY_Filter;

    
private:
    void InitializeCombineFilter(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
        const vector<ComPtr<ID3D11RenderTargetView>>& targets);
    void InitializeBloomFilter(const vector<ComPtr<ID3D11ShaderResourceView>>& resources,
        const vector<ComPtr<ID3D11RenderTargetView>>& targets);
    void InitializeGaussianFilter();
    

    
private:
    shared_ptr<StaticMeshData> MeshData;
    shared_ptr<VertexBuffer> VBuffer;
    shared_ptr<IndexBuffer> IBuffer;
    UINT IdxCount = 0;

    ComPtr<ID3D11Texture2D> PostProcess_TEX;
    ComPtr<ID3D11ShaderResourceView> PostProcessSRV;
    ComPtr<ID3D11RenderTargetView> PostProcessRTV;

    vector<ComPtr<ID3D11Texture2D>> bloomTexs;
    vector<ComPtr<ID3D11ShaderResourceView>> bloomSRVs;
    vector<ComPtr<ID3D11RenderTargetView>> bloomRTVs;

    ComPtr<ID3D11ShaderResourceView> GaussianX_SRV, GaussianY_SRV;
    ComPtr<ID3D11UnorderedAccessView> GaussianX_UAV, GaussianY_UAV;
    ComPtr<ID3D11Texture2D> GaussianX_TEX, GaussianY_TEX;

    
    
    ComPtr<ID3D11Texture2D> StagingTexture;

private:
    const int BloomLevels = 4;
};
