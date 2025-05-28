#pragma once
class ConstantBuffer;
class IndexBuffer;
class VertexBuffer;
struct VertexModel;
class BinaryReader;
class Material;
struct FTransform;
class Skeletal;


struct StaticMeshInfo
{
    string Name = "";

    Material* MaterialData = nullptr;
    Matrix* Transforms;

    UINT VertexCount = 0;
    VertexObject* Vertices = nullptr;

    UINT IndexCount = 0;
    UINT* Indices = nullptr;   
};

class StaticMesh
{
public:
    StaticMesh() = default;
    ~StaticMesh() = default;
    
    void Tick();
    void Render();

    void RenderMirror(const Matrix& refl);
    Material* GetMaterialData() const { return Data.MaterialData; }
    
protected:
    void SetWorld(const FTransform* InTransform);
    void CreateBuffer();

private:
    StaticMeshInfo Data;
    shared_ptr<FTransform> MeshWorld;
    shared_ptr<VertexBuffer> VBuffer = nullptr;
    shared_ptr<IndexBuffer> IBuffer = nullptr;
    
    friend class UStaticMeshComponent;
    friend class Converter;
};