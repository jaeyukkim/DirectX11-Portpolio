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
    
    virtual ~UStaticMeshComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    vector<shared_ptr<StaticMesh>> GetAllMeshes() {return m_Mesh;}

public:
    void ReverseIndices();
    vector<Material*> GetAllMaterials();
    int GetInstanceID() {return InstanceID;}
    void SetInstanceID(const int& InInstanceId) {InstanceID = InInstanceId;}

private:
    vector<shared_ptr<StaticMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;
    mutable bool bInitRenderComplete = false;
    int InstanceID;
    string MeshName;
    friend class Converter;

};

