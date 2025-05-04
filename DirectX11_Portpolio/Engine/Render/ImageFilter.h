#pragma once

class ConstantBuffer;

class ImageFilter {
public:
    ImageFilter() {};
    ImageFilter(wstring VSPath, wstring PSPath);

    void Initialize(wstring VSPath, wstring PSPath);
    void UpdateConstantBuffers();
    void DrawIndexed(UINT IndexCount) const;
    void SetShaderResources(const vector<ComPtr<ID3D11ShaderResourceView>>& resources);
    void SetRenderTargets(const vector<ComPtr<ID3D11RenderTargetView>>& targets);

public:
    struct ImageFilterConstData 
    {
        float dx;
        float dy;
        float threshold;
        float strength;
        float option1;
        float option2;
        float option3;
        float option4;
    };

    ImageFilterConstData FilterData = {};

protected:
    shared_ptr<Shader> renderer;
    shared_ptr<ConstantBuffer> CBuffer;
    D3D11_VIEWPORT m_viewport = {};

    // Do not delete pointers
    vector<ID3D11ShaderResourceView*> SRV;
    vector<ID3D11RenderTargetView*> RTV;
};