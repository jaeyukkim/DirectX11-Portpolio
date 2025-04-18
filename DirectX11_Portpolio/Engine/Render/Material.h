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

	
    void SetDiffuseMap(string InFilePath);
    void SetDiffuseMap(wstring InFilePath);

    void SetSpecularMap(string InFilePath);
    void SetSpecularMap(wstring InFilePath);

    void SetNormalMap(string InFilePath);
    void SetNormalMap(wstring InFilePath);

    Shader* GetRenderer() const { return Renderer.get(); }
    ConstantBuffer* GetConstantBuffer() const { return CBuffer.get(); }


private:
    struct Colors
    {
        Color Ambient = Color(0, 0, 0, 1);
        Color Diffuse = Color(1, 1, 1, 1);
        Color Specular = Color(0, 0, 0, 1);
        Color Emissive = Color(0, 0, 0, 1);
    } ColorData;

private:
    shared_ptr<ConstantBuffer> CBuffer = nullptr;
 //   ECB* sCBuffer = nullptr;	

private:
    shared_ptr<Shader> Renderer = nullptr;
    shared_ptr<Texture> Textures[MAX_MATERIAL_TEXTURE_COUNT];
    ComPtr<ID3D11ShaderResourceView> SRVs[MAX_MATERIAL_TEXTURE_COUNT];
 //   ESRV* sSRVs = nullptr;
};