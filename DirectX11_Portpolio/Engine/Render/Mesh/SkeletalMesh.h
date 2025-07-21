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
    Matrix* OffsetTransforms;

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
    
    
private:
    void SetWorld(const FTransform* InTransform);

private:
    SkeletalMeshInfo Data;
    shared_ptr<FTransform> MeshWorld;
    

    struct BoneDesc
    {
        Matrix Transforms[MAX_MODEL_TRANSFORM];
        Matrix OffsetTransforms[MAX_MODEL_TRANSFORM];

    } BoneData;
    
    struct BoneIDX
    {
        UINT BoneIndex;
        float Padding[3];
    } BoneIdxData;
    
private:
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;
    shared_ptr<ConstantBuffer> BoneBuffer = nullptr;
    shared_ptr<ConstantBuffer> BoneIndexCBuffer = nullptr;

    
private:
    friend class USkeletalMeshComponent;
    friend class Converter;
    friend class SkeletalMeshRenderProxy;
};
