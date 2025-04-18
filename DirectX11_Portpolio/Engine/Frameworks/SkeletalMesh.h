#pragma once




#define MAX_MODEL_TRANSFORM 250

class ConstantBuffer;
class IndexBuffer;
class VertexBuffer;
struct VertexModel;
class BinaryReader;
class Material;
struct FTransform;
class Skeletal;

class SkeletalMesh
{
public:
    SkeletalMesh();
    ~SkeletalMesh();

    void Tick();
    void Render();

private:
    void SetWorld(const FTransform* InTransform);

private:
    static void ReadFile(BinaryReader* InReader,const map<string, shared_ptr<Material>>& InMaterialTable,
        vector<shared_ptr<SkeletalMesh>>& OutMeshes);

private:
    void CreateBuffer();

private:
    string Name = "";

    Material* MaterialData = nullptr;

    UINT BoneIndex = 0;
    Skeletal* Bone = nullptr;
    Matrix* Transforms;

    UINT VertexCount = 0;
    VertexModel* Vertices = nullptr;

    UINT IndexCount = 0;
    UINT* Indices = nullptr;

private:
  //  Frame* FrameRender = nullptr;
    //Transform* World = nullptr;
    shared_ptr<FTransform> MeshWorld;
    shared_ptr<ConstantBuffer> BoneBuffer = nullptr;
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;

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
