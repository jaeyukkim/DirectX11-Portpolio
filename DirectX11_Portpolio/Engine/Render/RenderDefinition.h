#pragma once


class StructuredBuffer;
class VertexBuffer;
class Material;
class ConstantBuffer;
class IndexBuffer;

#define MAX_LIGHT_COUNT 30

enum class ELightType : UINT32
{
    None = (1 << 0),
    Directional = (1 << 1),
    Spot = (1 << 2),
    Point = (1 << 3),
    Lim = (1 << 4)
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
    LightCnt = 5,
    ImageFilterData = 6,
};


enum class EShaderResourceSlot : UINT8
{
    CubeMapTexture = 0,
    MaterialTexture = 4,
    LightMap = 11,
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
};


struct FViewRenderData
{
    shared_ptr<ConstantBuffer> ViewConstantBuffer;
    shared_ptr<ConstantBuffer> ReflactViewConstantBuffer;
};



static constexpr int MaxIBLMap = static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT);
struct FLightSceneRenderData
{
    shared_ptr<ConstantBuffer> LightCount;
    shared_ptr<StructuredBuffer> LightInformation;
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
struct FLightInformation
{
    ELightType LightType; //4
    int LightID; //4
    Vector3 strength = Vector3(2.0f, 2.0f, 2.0f);              // 12
    float fallOffStart = 0.0f;                     // 4
    Vector3 direction = Vector3(0.0f, -1.0f, 0.0f); // 12
    float fallOffEnd = 30.0f;                      // 4
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f); // 12
    float spotPower = 10.0f;                      // 4
    float innerCone = static_cast<float>(cos(XMConvertToRadians(20.0f)));
    float outerCone = static_cast<float>(cos(XMConvertToRadians(30.0f)));

};


struct FLightInfo
{
    int CurrentLightCnt = 0;
    float IBLStrength = 2.5f;
    float padding[2];
};


struct FViewContext
{
    Matrix View = Matrix::Identity;
    Matrix ViewInverse = Matrix::Identity;
    Matrix Projection = Matrix::Identity;
    Matrix ViewProjection = Matrix::Identity;
    Vector3 EyePos = Vector3(0, 0, 0);
    float padding;
};

