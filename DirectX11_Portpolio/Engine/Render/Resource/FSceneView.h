#pragma once


struct FSceneView
{
public:
    FSceneView() = default;
    ~FSceneView() = default;


public:
    static FSceneView* Get();
    static void Create();
    static void Destroy();



public:
    void UpdateSceneView(const FViewContext& InContext);
    void UpdateReflactRow(const Matrix InReflactRow);
    FViewContext* GetSceneViewContext() { return &Context; }
    void UpdateIBLStrength(float InIBLStrength) {LightInfoCbuffer.IBLStrength = InIBLStrength;}
    FLightInfo* GetLightInfo() { return &LightInfoCbuffer; }

public:
    void AddToLightMap(FLightInformation* InLightInfo);
    void UpdateLightMap(FLightInformation& InLightInfo);
    void UpdateSkyLight(ECubeMapType IBLType, ID3D11ShaderResourceView* InIBLSRV);

private:
    FViewContext Context;
    FViewContext ReflactContext;
    shared_ptr<ConstantBuffer> ViewConstantBuffer;
    shared_ptr<ConstantBuffer> ReflactViewConstantBuffer;

private:
    FLightInfo LightInfoCbuffer;
    
    static atomic<uint8_t> LightCounter;
    
    shared_ptr<ConstantBuffer> LightCountCBuffer;
    unordered_map<uint8_t, FLightInformation> LightMap;
    vector<FLightInformation> CachedLights;
    shared_ptr<StructuredBuffer> LightConstantBuffer;


    static constexpr int MaxIBLMap = static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT);
    ComPtr<ID3D11ShaderResourceView> IBLSRV[MaxIBLMap] = { nullptr };

private:
    static FSceneView* Instance; 
    friend class LightSceneRenderProxy;
    friend class ViewRenderProxy;
};
