#pragma once


class Shader;


#define MATERIAL_TEXTURE_DIFFUSE 0
#define MATERIAL_TEXTURE_SPECULAR 1
#define MATERIAL_TEXTURE_NORMAL 2
#define MAX_MATERIAL_TEXTURE_COUNT 3

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
    void SetSamplerDesc(const D3D11_SAMPLER_DESC& InsampDesc);
	
    void SetDiffuseMap(string InFilePath);
    void SetDiffuseMap(wstring InFilePath);

    void SetSpecularMap(string InFilePath);
    void SetSpecularMap(wstring InFilePath);

    void SetNormalMap(string InFilePath);
    void SetNormalMap(wstring InFilePath);

    Shader* GetRenderer() const { return Renderer.get(); }
    ConstantBuffer* GetConstantBuffer() const { return ColorConstantBuffer.get(); }
    const D3D11_SAMPLER_DESC& GetSamplerDesc() const { return SampDesc;  }


public:
    void SetUVTiling(Vector2 InUV_Tiling);

private:
    struct Colors
    {
        Color Ambient = Color(0.f, 0.f, 0.f, 1.f);
        Color Diffuse = Color(1.f, 1.f, 1.f, 1.f);
        Color Specular = Color(0.f, 0.f, 0.f, 1.f);
        Color Emissive = Color(0.f, 0.f, 0.f, 1.f);
        Vector2 UV_Tiling = Vector2(1.0f, 1.0f);
        Vector2 UV_Offset = Vector2(0.0f, 0.0f);
        Vector4 padding[3];
    } ColorData;

private:
    shared_ptr<ConstantBuffer> ColorConstantBuffer = nullptr;
   


private:
    shared_ptr<Shader> Renderer = nullptr;
    shared_ptr<Texture> Textures[MAX_MATERIAL_TEXTURE_COUNT];
    ComPtr<ID3D11ShaderResourceView> SRVs[MAX_MATERIAL_TEXTURE_COUNT];
    D3D11_SAMPLER_DESC SampDesc;
 //   ESRV* sSRVs = nullptr;
};