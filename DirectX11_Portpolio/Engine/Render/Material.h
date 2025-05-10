#pragma once


class Shader;

enum class MaterialMapType : UINT8
{
    ALBEDO = 0,
    METALLIC = 1,
    ROUGHNESS = 2,
    NORMAL = 3,
    AMBIENTOCCLUSION = 4,
    EMISSIVE = 5,
    HEIGHT = 6,
    MAX_TEXTURE_COUNT = 7,
};


enum class CubeMapType : UINT8
{
    ENVTEX = 0,
    SPECULAR = 1,
    IRRADIENCE = 2,
    BRDF = 3,
    MAX_CUBEMAP_TEXTURE_COUNT = 4,
};

class Material
{
public:
    Material();
    ~Material() = default;

    void Render();

private:
    void Initialize();
    

public:
    void SetAlbedo(const Color& InColor) { MaterialDesc.Albedo = InColor; }
    void SetRoughness(const float InColor) { MaterialDesc.Roughness = InColor; }
    void SetMetallic(const float InColor) { MaterialDesc.Metallic = InColor; }
    void SetEmissive(const Color& InColor) { MaterialDesc.Emissive = InColor; }
    
    Shader* GetRenderer() const { return Renderer.get(); }
    ConstantBuffer* GetConstantBuffer() const { return ColorConstantBuffer.get(); }
    ESamplerSlot GetSamplerDesc() const { return SampDesc; }
    ID3D11ShaderResourceView* GetSRV(MaterialMapType InMaterialMapType);

public:
    void SetUVTiling(Vector2 InUV_Tiling);
    bool CheckingMaterialMap(MaterialMapType InMaterialMapType);
    void SetTextureMap(wstring InFilePath, MaterialMapType InMaterialMapType);
    void SetIsCubeMap(bool InbCubeMap) { bCubeMap = InbCubeMap; }
    void SetSamplerDesc(ESamplerSlot InsampDesc);
    

public:
    struct MaterialDescription
    {
        Color Albedo = Color(1.f, 1.f, 1.f, 1.f);   //16
        
        float Roughness = 0.0f;     //4
        float Metallic = 0.0f;      //4
        Vector2 padding = Vector2(0.0f, 0.0f);  //8
        
        Color Emissive = Color(0.f, 0.f, 0.f, 1.f); //16
        
        Vector2 UV_Tiling = Vector2(1.0f, 1.0f);    //8
        Vector2 UV_Offset = Vector2(0.0f, 0.0f);    //8
        
        int bUseAlbedoMap = false;  //4
        int bUseNormalMap = false;  //4
        int bUseAOMap = false;  //4
        int bInvertNormalMapY = false;  //4
        
        int bUseMetallicMap = false;    //4
        int bUseRoughnessMap = false;    //4
        int bUseEmissiveMap = false;    //4
        int padding2;   //4
    } MaterialDesc;

public:
    MaterialDescription* GetMatDesc() { return &MaterialDesc; }

private:
    static constexpr int MAX_TEXTURE_COUNT = static_cast<int>(MaterialMapType::MAX_TEXTURE_COUNT);
    shared_ptr<Shader> Renderer = nullptr;
    shared_ptr<Texture> Textures[MAX_TEXTURE_COUNT];
    ComPtr<ID3D11ShaderResourceView> SRVs[MAX_TEXTURE_COUNT];
    ESamplerSlot SampDesc = ESamplerSlot::DefaultSampler;
    shared_ptr<ConstantBuffer> ColorConstantBuffer = nullptr;

private:
    bool bCubeMap = false;
 
};