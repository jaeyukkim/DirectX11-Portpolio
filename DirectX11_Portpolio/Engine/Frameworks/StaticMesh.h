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
    virtual void DrawIndexed();
    virtual void RenderMirror(const Matrix& refl);
    Material* GetMaterialData() { return MaterialData; }
protected:
    virtual void SetWorld(const FTransform* InTransform);
    virtual void BindRenderStage();
    virtual void CreateBuffer();
    
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
    shared_ptr<FTransform> MeshWorld;
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;

private:
    friend class UStaticMeshComponent;
};