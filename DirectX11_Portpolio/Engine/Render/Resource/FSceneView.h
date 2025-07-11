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

    void CreateFrustum(const FViewContext& InContext);
    void CreateFrustum(vector<XMVECTOR>& InFrustum, const Matrix& viewProj,
        const shared_ptr<ConstantBuffer>& InFrustumCBuffer);
    vector<XMVECTOR>& GetFrustum() {return Frustum;}
    
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
    FViewContext FrustumView;
    FViewContext ReflactFrustumView;
    
    vector<XMVECTOR> Frustum;
    vector<XMVECTOR> ReflectFrustum;


    shared_ptr<ConstantBuffer> ReflectFrustumCBuffer;
    shared_ptr<ConstantBuffer> FrustumCBuffer;
    shared_ptr<ConstantBuffer> ViewCBuffer;
    shared_ptr<ConstantBuffer> ReflactViewCBuffer;
    shared_ptr<ConstantBuffer> LightViewCBuffer[MAX_LIGHT_COUNT];


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
