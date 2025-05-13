#pragma once
#include "Material.h"
#define MAX_LIGHT_COUNT 30



struct FViewContext
{
    Matrix View = Matrix::Identity;
    Matrix ViewInverse = Matrix::Identity;
    Matrix Projection = Matrix::Identity;
    Matrix ViewProjection = Matrix::Identity;
    Vector3 EyePos = Vector3(0, 0, 0);
    float padding;
};

enum class ELightType : UINT32
{
    None,
    CubeMap,
    Directional,
    Spot,
    Point,
    Lim
};

// 조명 정보
struct LightInformation
{
    ELightType LightType; //4
    int LightID; //4
    Vector3 strength = Vector3(0.5f, 0.5f, 0.5f);              // 12
    float fallOffStart = 0.0f;                     // 4
    Vector3 direction = Vector3(0.0f, -1.0f, 0.0f); // 12
    float fallOffEnd = 10.0f;                      // 4
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f); // 12
    float spotPower = 0.5f;                      // 4
    float innerCone = static_cast<float>(cos(XMConvertToRadians(20.0f)));
    float outerCone = static_cast<float>(cos(XMConvertToRadians(30.0f)));

};



struct FSceneView
{
public:
    FSceneView() = default;
    ~FSceneView() = default;


public:
    static FSceneView* Get();
    static void Create();
    static void Destroy();
    static void PreRender();


public:
    void UpdateSceneView(const FViewContext& InContext);
    D3D_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return Instance->IAPrimitive; }
    FViewContext& GetSceneViewContext() { return Context; }

public:
    void AddToLightMap(LightInformation* InLightInfo);
    void UpdateLightMap(LightInformation& InLightInfo);
    void UpdateSkyLight(CubeMapType IBLType, ID3D11ShaderResourceView* InIBLSRV);

private:
    FViewContext Context;
    shared_ptr<ConstantBuffer> ViewConstantBuffer;

private:
    struct CLightCnt
    {
        int CurrentLightCnt = 0;
        float padding[3];
    }LightCntCbuffer;
    
    static atomic<uint8_t> LightCounter;
    
    shared_ptr<ConstantBuffer> LightCountCBuffer;
    unordered_map<uint8_t, LightInformation> LightMap;
    vector<LightInformation> CachedLights;
    shared_ptr<StructuredBuffer> LightConstantBuffer;


    static constexpr int MaxIBLMap = static_cast<int>(CubeMapType::MAX_CUBEMAP_TEXTURE_COUNT);
    ComPtr<ID3D11ShaderResourceView> IBLSRV[MaxIBLMap] = { nullptr };

private:
    static FSceneView* Instance; 
    

private:
    D3D_PRIMITIVE_TOPOLOGY IAPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
