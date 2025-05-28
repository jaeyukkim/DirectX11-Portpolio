#pragma once
#include "UPrimitiveComponent.h"
#include "Render/RenderData.h"

class StaticMesh;
class Material;

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    UStaticMeshComponent(wstring InFileName, bool bOverwrite = false);
    virtual ~UStaticMeshComponent() = default;

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent() override;
    virtual void RenderMirror(const Matrix& refl);
 

public:
    void ReverseIndices();
    vector<Material*> GetAllMaterials();

private:
    vector<shared_ptr<StaticMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;

private:
    mutable bool bInitRenderComplete = false;
    friend class Converter;
};

