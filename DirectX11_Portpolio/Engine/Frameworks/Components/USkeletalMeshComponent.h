#pragma once

#include "UPrimitiveComponent.h"

class FClipData;
class Material;
class SkeletalMesh;
class Skeletal;

struct SkeletalMeshCreateInfo
{
    vector<string> exportAnimName;
    bool bReadAnimation = true;
    bool bExportAnimation = false;
    bool bOverWrite = false;
};


class USkeletalMeshComponent : public UPrimitiveComponent
{
public:
    USkeletalMeshComponent(wstring InFileName, const SkeletalMeshCreateInfo info = SkeletalMeshCreateInfo());
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
    unordered_map<string, shared_ptr<Skeletal>> ReadBoneTable;
    vector<shared_ptr<FClipData>> Animations;
    
    DirectX::SimpleMath::Matrix Transforms[MAX_MODEL_TRANSFORM];
    int* InstanceID;
    string MeshName;

private:
    friend class Converter;
    friend class AnimationTexture;
};
 