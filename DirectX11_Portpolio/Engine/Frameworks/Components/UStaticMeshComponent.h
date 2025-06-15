#pragma once
#include "UPrimitiveComponent.h"

class StaticMesh;
class Material;

struct StaticMeshCreateInfo
{
    bool bIsSkyBox = false;
    bool bIsMirror = false;
    bool bOverWrite = false;
};

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    UStaticMeshComponent(wstring InFileName, StaticMeshCreateInfo info = StaticMeshCreateInfo());
    
    virtual ~UStaticMeshComponent() = default;

public:
    virtual void TickComponent(float deltaTime) override;
    vector<shared_ptr<StaticMesh>> GetAllMeshes() {return m_Mesh;}

public:
    void ReverseIndices();
    vector<Material*> GetAllMaterials();

private:
    vector<shared_ptr<StaticMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;
    mutable bool bInitRenderComplete = false;
    
    
    friend class Converter;

};

