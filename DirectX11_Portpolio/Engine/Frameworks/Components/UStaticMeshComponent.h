#pragma once
#include "UPrimitiveComponent.h"

class StaticMesh;
class Material;

struct StaticMeshCreateInfo
{
    bool bIsSkyBox = false;
    bool bIsMirror = false;
};

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    UStaticMeshComponent(wstring InMeshName, StaticMeshCreateInfo info = StaticMeshCreateInfo());
    
    virtual ~UStaticMeshComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    vector<shared_ptr<StaticMesh>> GetAllMeshes() {return m_Mesh;}

public:
    void ReverseIndices();
    vector<Material*> GetAllMaterials();
    int GetInstanceID() {return *InstanceID;}
    void SetInstanceID(int* InInstanceId) {InstanceID = InInstanceId;}

private:
    vector<shared_ptr<StaticMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;
    mutable bool bInitRenderComplete = false;
    int* InstanceID = nullptr;
    string MeshName;

private:
    friend class Converter;
    
};

