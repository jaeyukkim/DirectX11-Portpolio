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

class SkeletalMesh : public StaticMesh
{
public:
    SkeletalMesh();
    virtual ~SkeletalMesh();

    virtual void Tick() override;
    virtual void Render(bool bUsePreRender = false) override;

private:
    virtual void SetWorld(const FTransform* InTransform) override;
    virtual void BindRenderStage() override;
private:
    static void ReadFile(BinaryReader* InReader,const map<string, shared_ptr<Material>>& InMaterialTable,
        vector<shared_ptr<SkeletalMesh>>& OutMeshes);

protected:
    virtual void CreateBuffer() override;

private:
    UINT BoneIndex = 0;
    Skeletal* Bone = nullptr;
    VertexModel* ModelVertices = nullptr;
private:
    shared_ptr<ConstantBuffer> BoneBuffer = nullptr;
    

private:
    struct BoneDesc
    {
        Matrix Transforms[MAX_MODEL_TRANSFORM];
        UINT BoneIndex;
        float Padding[3];
    } BoneData;


private:
    friend class USkeletalMeshComponent;
};
