#pragma once


class StructuredBuffer;
class VertexBuffer;
class Material;
class ConstantBuffer;
class IndexBuffer;

#define MAX_LIGHT_COUNT 9
#define MAX_SHADOW_COUNT 9
#define MAX_INSTANCE_SIZE 1000

enum ELightType : UINT
{
    LT_None = 0,
    LT_Directional = (1 << 0),
    LT_Spot = (1 << 1),
    LT_Point = (1 << 2),
    LT_Lim = (1 << 3),
    LT_Halo = (1 << 4),
    LT_UseShadow = (1 << 5)
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
    PPT_BloomDown,
    PPT_BloomUp,
    PPT_Combine,
    PPT_GaussianX,
    PPT_GaussianY
};

enum class EConstBufferSlot : UINT8
{
    CB_MaterialDesc = 0,
    CB_World = 1,
    CB_ViewContext = 2,
    CB_Bone = 3,
    CB_MaterialTex = 4,
    CB_LightObjects = 5,
    CB_LightInfo = 6,
    CB_ImageFilterData = 7,
    CB_PostEffectData = 8
};


enum class EShaderResourceSlot : UINT8
{
    ERS_CubeMapTexture = 0,
    ERS_MaterialTexture = 4,
    ERS_ShadowMap = 11,
    ERS_PostEffect = 20,
    ERS_World = 25
    
};


enum ESamplerSlot : UINT8
{
    LinearWrapSampler = 0,
    LinearClampSampler = 1,
    shadowPointSampler = 2,
    shadowCompareSampler = 3,
    MaxSamplerSlot = 4
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
    shared_ptr<ConstantBuffer> LightsCBuffer;
    shared_ptr<ConstantBuffer> LightInfoCBuffer;
    ID3D11ShaderResourceView** IBLSRVRef = nullptr;
};


struct WorldBufferDesc
{
    Matrix World;
};

struct FSM_InstDataCPU
{
    Matrix Transform;
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
    Material* MaterialData;
};
struct FSKM_InstDataCPU
{
    Matrix Transform;
};



// 조명 정보
struct FLight
{
    Matrix viewProj;
    Matrix invProj;


    Vector3 strength = Vector3(2.0f, 2.0f, 2.0f);
    float padding0 = 0.0f;
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
    float padding1 = 0.0f;;
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
    float padding2 = 0.0f;;


    UINT LightType = ELightType::LT_None; 
    int LightID; 
    float radius = 0.5f;
    float spotPower = 1.0f;


    float fallOffStart = 0.0f;  
    float fallOffEnd = 30.0f;                      
    float innerCone = static_cast<float>(cos(XMConvertToRadians(20.0f)));
    float outerCone = static_cast<float>(cos(XMConvertToRadians(30.0f)));

};


struct FLightObjects
{
    FLight Lights[MAX_LIGHT_COUNT];
};

struct FLightInfo
{
    int CurrentLightCnt = 0;
    float IBLStrength = 2.5f;
    int ShadowCount = 0;
    float padding2 = 0.0f;
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