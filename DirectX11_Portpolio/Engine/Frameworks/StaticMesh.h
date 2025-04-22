#pragma once
class ConstantBuffer;
class IndexBuffer;
class VertexBuffer;
struct VertexModel;
class BinaryReader;
class Material;
struct FTransform;
class Skeletal;

class StaticMesh
{
public:
    StaticMesh();
    virtual ~StaticMesh();

public:
    virtual void Tick();
    virtual void Render();

protected:
    virtual void SetWorld(const FTransform* InTransform);
    virtual void CreateBuffer();
    virtual void BindRenderStage();

private:
    static void ReadFile(BinaryReader* InReader,
        const map<string, shared_ptr<Material>>& InMaterialTable, vector<shared_ptr<StaticMesh>>& OutMeshes);


protected:
    string Name = "";

    Material* MaterialData = nullptr;
    Matrix* Transforms;

    UINT VertexCount = 0;
    VertexObject* Vertices = nullptr;

    UINT IndexCount = 0;
    UINT* Indices = nullptr;

protected:
    //  Frame* FrameRender = nullptr;
      //Transform* World = nullptr;
    shared_ptr<FTransform> MeshWorld;
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;

private:
  


private:
    friend class UStaticMeshComponent;
};