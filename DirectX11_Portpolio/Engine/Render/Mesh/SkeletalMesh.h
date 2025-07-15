#pragma once
#include "Render/Resource/AnimationData.h"


class ConstantBuffer;
class IndexBuffer;
class VertexBuffer;
struct VertexModel;
class BinaryReader;
class Material;
struct FTransform;
class Skeletal;

struct SkeletalMeshInfo
{
    string Name = "";

    Material* MaterialData = nullptr;
    Matrix* Transforms;

    UINT VertexCount = 0;
    VertexModel* ModelVertices = nullptr;

    UINT IndexCount = 0;
    UINT* Indices = nullptr;  

    UINT BoneIndex = 0;
    Skeletal* Bone = nullptr;
    
    FAABB AABB;
};

class SkeletalMesh
{
public:
    SkeletalMesh() = default;
    ~SkeletalMesh() = default;

    void Tick();
    void Render();
    Material* GetMaterialData() const { return Data.MaterialData; }
    
protected:
    void CreateBuffer();
    void CreateAnimationBuffer();
    
private:
    void SetWorld(const FTransform* InTransform);

private:
    SkeletalMeshInfo Data;
    shared_ptr<FTransform> MeshWorld;
    AnimationBlendingDesc BlendingData[MAX_INSTANCE_SIZE];

    struct BoneDesc
    {
        Matrix Transforms[MAX_MODEL_TRANSFORM];
        UINT BoneIndex;
        float Padding[3];
    } BoneData;
    
private:
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;
    shared_ptr<ConstantBuffer> BoneBuffer = nullptr;
    ComPtr<ID3D11ShaderResourceView> ClipsSRV = nullptr;
    shared_ptr<ConstantBuffer> BlendDataCBuffer;

    
private:
    friend class USkeletalMeshComponent;
    friend class Converter;
    friend class SkeletalMeshRenderProxy;
};
