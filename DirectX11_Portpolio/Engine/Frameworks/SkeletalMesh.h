#pragma once

#include "StaticMesh.h"


#define MAX_MODEL_TRANSFORM 250

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
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;
    shared_ptr<ConstantBuffer> BoneBuffer = nullptr;
    
    struct BoneDesc
    {
        Matrix Transforms[MAX_MODEL_TRANSFORM];
        UINT BoneIndex;
        float Padding[3];
    } BoneData;
    
    friend class USkeletalMeshComponent;
    friend class Converter;
};
