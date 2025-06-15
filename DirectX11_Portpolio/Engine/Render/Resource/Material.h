#pragma once


class FGlobalPSO;



class Material
{
public:
    Material();
    ~Material() = default;

    void BindMaterial();


public:
    void SetAlbedo(const Color& InColor) { MaterialDesc.Albedo = InColor; }
    void SetRoughness(const float InColor) { MaterialDesc.Roughness = InColor; }
    void SetMetallic(const float InColor) { MaterialDesc.Metallic = InColor; }
    void SetEmissive(const Color& InColor) { MaterialDesc.Emissive = InColor; }
    
    ConstantBuffer* GetConstantBuffer() const { return ColorConstantBuffer.get(); }
    ID3D11ShaderResourceView* GetSRV(EMaterialMapType InMaterialMapType);
    ID3D11ShaderResourceView* GetSkyMapSRV(ECubeMapType InCubeMapType);

public:
    void SetUVTiling(Vector2 InUV_Tiling);
    bool CheckingMaterialMap(EMaterialMapType InMaterialMapType);
    void SetTextureMap(wstring InFilePath, EMaterialMapType InMaterialMapType);
    void SetIsCubeMap(bool InbCubeMap) { bCubeMap = InbCubeMap; }


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
    static constexpr int MAX_TEXTURE_COUNT = static_cast<int>(EMaterialMapType::MAX_TEXTURE_COUNT);
    shared_ptr<Texture> Textures[MAX_TEXTURE_COUNT];
    ComPtr<ID3D11ShaderResourceView> SRVs[MAX_TEXTURE_COUNT];
    shared_ptr<ConstantBuffer> ColorConstantBuffer = nullptr;

private:
    bool bCubeMap = false;
 
};