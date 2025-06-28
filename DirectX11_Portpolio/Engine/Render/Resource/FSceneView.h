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
    void UpdateReflactView(const Matrix InReflactRow);
    void UpdateLightView(FLight* InLightInfo);
    
    FViewContext* GetSceneViewContext() { return &DefaultView; }
    void UpdateIBLStrength(float InIBLStrength) {LightInfo.IBLStrength = InIBLStrength;}
    FLightObjects* GetLights() { return &Lights; }
    FLightInfo* GetLightInfo() {return &LightInfo;}

public:
    void AddToLightMap(FLight* InLightInfo);
    void DeleteFromLightMap(int InLightID);
    void UpdateLightMap(FLight* InLightInfo);
    void UpdateSkyLight(ECubeMapType IBLType, ID3D11ShaderResourceView* InIBLSRV);
    
private:
    FViewContext DefaultView;
    FViewContext ReflactView;
    FViewContext ShadowView[MAX_LIGHT_COUNT];
    shared_ptr<ConstantBuffer> ViewConstantBuffer;
    shared_ptr<ConstantBuffer> ReflactViewConstantBuffer;
    shared_ptr<ConstantBuffer> LightViewConstantBuffer[MAX_LIGHT_COUNT];


private:
    FLightObjects Lights;
    FLightInfo LightInfo;
    static atomic<UINT8> LightCounter;
    static atomic<UINT8> ShadowCounter;
    shared_ptr<ConstantBuffer> LightsCBuffer;
    shared_ptr<ConstantBuffer> LightsInfoCBuffer;
  
    
    static constexpr int MaxIBLMap = static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT);
    ComPtr<ID3D11ShaderResourceView> IBLSRV[MaxIBLMap] = { nullptr };

private:
    static FSceneView* Instance; 
    friend class LightSceneRenderProxy;
    friend class ViewRenderProxy;
};
