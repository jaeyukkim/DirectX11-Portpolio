#pragma once


class StructuredBuffer;
class VertexBuffer;
class Material;
class ConstantBuffer;
class IndexBuffer;

#define MAX_LIGHT_COUNT 30

enum ELightType : UINT
{
    LT_None = 0,
    LT_Directional = (1 << 0),
    LT_Spot = (1 << 1),
    LT_Point = (1 << 2),
    LT_Lim = (1 << 3),
    LT_UseShadow = (1 << 4)
};


enum ERenderProxyType : UINT8
{
    RPT_None,
    RPT_LightScene,
    RPT_View,
    RPT_SkeletalMesh,
    RPT_StaticMesh,
    RPT_SkyBox,
    RPT_Mirror
};


enum class EMaterialMapType : UINT8
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


enum class ECubeMapType : UINT8
{
    ENVTEX = 0,
    SPECULAR = 1,
    IRRADIENCE = 2,
    BRDF = 3,
    MAX_CUBEMAP_TEXTURE_COUNT = 4,
};


enum EPostProcessType : UINT8
{
    None,
    BloomDown,
    BloomUp,
    Combine
};

enum class EConstBufferSlot : UINT8
{
    MaterialDesc = 0,
    World = 1,
    ViewContext = 2,
    Bone = 3,
    MaterialTex = 4,
    LightInfo = 5,
    ImageFilterData = 6,
    PostEffectData = 7
};


enum class EShaderResourceSlot : UINT8
{
    CubeMapTexture = 0,
    MaterialTexture = 4,
    LightMap = 11,
    PostEffect = 20
};


enum ESamplerSlot : UINT8
{
    LinearWrapSampler = 0,
    LinearClampSampler = 1,
    MaxSamplerSlot = 2
};


struct FRenderOption
{
    bool bDefaultDraw = false;
    bool bIsMirror = false;
    bool bIsWire = false;
    bool bBlendOn = false;
    bool bStencilOn = false;
    bool bDepthOnly = false;
};


struct FViewRenderData
{
    shared_ptr<ConstantBuffer> ViewConstantBuffer;
    shared_ptr<ConstantBuffer> ReflactViewConstantBuffer;
    shared_ptr<ConstantBuffer> LightViewConstantBuffer[MAX_LIGHT_COUNT];
};



static constexpr int MaxIBLMap = static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT);
struct FLightSceneRenderData
{
    shared_ptr<ConstantBuffer> LightInformation;
    ID3D11ShaderResourceView** IBLSRVRef = nullptr;
};


struct FStaticMeshRenderData
{
    UINT IndexCount;
    shared_ptr<VertexBuffer> VBuffer;
    shared_ptr<IndexBuffer> IBuffer;
    shared_ptr<ConstantBuffer> Transform;
    Material* MaterialData;
};


using FSkyBoxRenderData = FStaticMeshRenderData;

//리플렉션 로우 설정하는거 mirror에서 설정하도록 바꾸기
struct FMirrorRenderData
{
    FStaticMeshRenderData MeshData;
    Matrix* ReflectionRow = nullptr;
    array<float, 4>* BlendFactor = nullptr;
};


struct FSkeletalMeshRenderData
{
    UINT IndexCount;
    shared_ptr<VertexBuffer> VBuffer;
    shared_ptr<IndexBuffer> IBuffer;
    shared_ptr<ConstantBuffer> BoneBuffer;
    shared_ptr<ConstantBuffer> Transform;
    Material* MaterialData;
};


// 조명 정보
__declspec(align(16)) struct FLightInformation
{
    UINT LightType = ELightType::LT_None; 
    int LightID; 
    float padding0[2] = { 0 };

    Vector3 strength = Vector3(2.0f, 2.0f, 2.0f);     
    float fallOffStart = 0.0f;  

    Vector3 direction = Vector3(0.0f, -1.0f, 0.0f); 
    float fallOffEnd = 30.0f;                      

    Vector3 position = Vector3(0.0f, 0.0f, -2.0f); 
    float spotPower = 10.0f;                      

    float innerCone = static_cast<float>(cos(XMConvertToRadians(20.0f)));
    float outerCone = static_cast<float>(cos(XMConvertToRadians(30.0f)));
    float padding1[2] = { 0 };
};


struct FLightInfo
{
    FLightInformation Lights[MAX_LIGHT_COUNT];
    int CurrentLightCnt = 0;
    float IBLStrength = 2.5f;
    float padding[2] = { 0 };
};


__declspec(align(256)) struct FViewContext
{
    Matrix View = Matrix::Identity;
    Matrix ViewInverse = Matrix::Identity;
    Matrix Projection = Matrix::Identity;
    Matrix ProjectionInverse = Matrix::Identity;
    Matrix ViewProjection = Matrix::Identity;
    Vector3 EyePos = Vector3(0, 0, 0);
    float padding;
};


__declspec(align(256)) struct PostEffectsData
{
    int mode = 1; // 1: Rendered image, 2: DepthOnly
    float DepthScale = 1.0f;
    float FogStrength = 0.0f;
};


struct FShadowMapResources
{
    ComPtr<ID3D11Texture2D> ShadowBuffer;
    ComPtr<ID3D11DepthStencilView> ShadowDSV;
    ComPtr<ID3D11ShaderResourceView> ShadowSRV;
};