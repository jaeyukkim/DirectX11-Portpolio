#pragma once

class PostEffect
{
public:
    PostEffect();
    void Initialize();
    void Render();
    PostEffectsData& GetPostEffectData() { return Data; }

    
private:
    PostEffectsData Data;
    shared_ptr<ConstantBuffer> PostEffectCBuffer;
    vector<ID3D11ShaderResourceView*> PostEffectsSRVs;


private:
    shared_ptr<VertexTextureData> MeshData;
    shared_ptr<VertexBuffer> VBuffer;
    shared_ptr<IndexBuffer> IBuffer;
    UINT IdxCount = 0;
};
