#pragma once


class Shader;

#define MATERIAL_TEXTURE_AMBIENT 0
#define MATERIAL_TEXTURE_DIFFUSE 1
#define MATERIAL_TEXTURE_SPECULAR 2
#define MATERIAL_TEXTURE_NORMAL 3
#define MAX_MATERIAL_TEXTURE_COUNT 4
#define MAX_CUBEMAP_TEXTURE_COUNT 3

class Material
{
public:
    Material();
    ~Material();

    void Render();

private:
    void Initialize();

public:
    void SetAmbient(const Color& InColor) { ColorData.Ambient = InColor; }
    void SetDiffuse(const Color& InColor) { ColorData.Diffuse = InColor; }
    void SetSpecular(const Color& InColor) { ColorData.Specular = InColor; }
    void SetEmissive(const Color& InColor) { ColorData.Emissive = InColor; }
    void SetShininess(const float InShininess) { ColorData.Shininess = InShininess; }
    void SetSamplerDesc(const D3D11_SAMPLER_DESC& InsampDesc);
	
    void SetAmbientMap(string InFilePath);
    void SetAmbientMap(wstring InFilePath);

    void SetDiffuseMap(string InFilePath);
    void SetDiffuseMap(wstring InFilePath);

    void SetSpecularMap(string InFilePath);
    void SetSpecularMap(wstring InFilePath);

    void SetNormalMap(string InFilePath);
    void SetNormalMap(wstring InFilePath);

    void SetIsCubeMap(bool InbCubeMap) { bCubeMap = InbCubeMap; }
    Shader* GetRenderer() const { return Renderer.get(); }
    ConstantBuffer* GetConstantBuffer() const { return ColorConstantBuffer.get(); }
    const D3D11_SAMPLER_DESC& GetSamplerDesc() const { return SampDesc;  }
    ID3D11ShaderResourceView* GetSRV(int materialType);

public:
    void SetUVTiling(Vector2 InUV_Tiling);

private:
    struct Colors
    {
        Color Ambient = Color(0.f, 0.f, 0.f, 1.f);
        Color Diffuse = Color(1.f, 1.f, 1.f, 1.f);
        Color Specular = Color(0.f, 0.f, 0.f, 1.f);
        Color Emissive = Color(0.f, 0.f, 0.f, 1.f);
        float Shininess = 0.0f;
        Vector2 UV_Tiling = Vector2(1.0f, 1.0f);
        Vector2 UV_Offset = Vector2(0.0f, 0.0f);
        Vector4 padding[2];
        float padding2[3];
    } ColorData;

private:
    
   


private:
    shared_ptr<Shader> Renderer = nullptr;
    shared_ptr<Texture> Textures[MAX_MATERIAL_TEXTURE_COUNT];
    ComPtr<ID3D11ShaderResourceView> SRVs[MAX_MATERIAL_TEXTURE_COUNT];
    D3D11_SAMPLER_DESC SampDesc;
    shared_ptr<ConstantBuffer> ColorConstantBuffer = nullptr;

private:
    bool bCubeMap = false;
 
};