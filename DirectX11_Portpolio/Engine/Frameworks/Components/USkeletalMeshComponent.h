#pragma once

#include "UPrimitiveComponent.h"

class Material;
class SkeletalMesh;
class Skeletal;


class USkeletalMeshComponent : public UPrimitiveComponent
{
public:
    USkeletalMeshComponent(wstring InFileName, bool bOverwrite = false);
    virtual ~USkeletalMeshComponent() override;
    virtual void TickComponent(float deltaTime) override;
    
    vector<Material*> GetAllMaterials();
    vector<shared_ptr<SkeletalMesh>> GetAllMeshes() {return m_Mesh;}
    
    int GetInstanceID() {return *InstanceID;}
    void SetInstanceID(int* InInstanceId) {InstanceID = InInstanceId;}

private:
    vector<shared_ptr<Skeletal>> Bones;
    vector<shared_ptr<SkeletalMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;

    DirectX::SimpleMath::Matrix Transforms[MAX_MODEL_TRANSFORM];
    int* InstanceID;
    string MeshName;
    friend class Converter;
};
 