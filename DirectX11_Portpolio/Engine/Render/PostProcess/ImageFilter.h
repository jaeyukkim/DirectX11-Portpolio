#pragma once

class ConstantBuffer;

class ImageFilter 
{
public:
    ImageFilter() = default;
    
    void Initialize(EPostProcessType type, int width, int height);


    void UpdateConstantBuffers();
    void DrawIndexed(UINT IndexCount) const;
    void SetShaderResources(const vector<ComPtr<ID3D11ShaderResourceView>>& resources);
    void SetUAVResources(const ComPtr<ID3D11UnorderedAccessView>& resources);
    void SetRenderTargets(const vector<ComPtr<ID3D11RenderTargetView>>& targets);
    void Dispatch();
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
    shared_ptr<ConstantBuffer> CBuffer;
    D3D11_VIEWPORT m_viewport = {};
    
    vector<ID3D11ShaderResourceView*> SRV;
    vector<ID3D11RenderTargetView*> RTV;
    ComPtr<ID3D11UnorderedAccessView> UAV;

private:
    EPostProcessType PostProcessType;
    UINT ThreadGroupX;
    UINT ThreadGroupY;

};