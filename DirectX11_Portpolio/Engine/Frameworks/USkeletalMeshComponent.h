#pragma once
#include "UPrimitiveComponent.h"
#include "Skeletal.h"
#include "SkeletalMesh.h"



class USkeletalMeshComponent : public UPrimitiveComponent
{
public:
    USkeletalMeshComponent(wstring InFileName, bool bOverwrite = false);
    virtual ~USkeletalMeshComponent() override = default;
    
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent() override;
    
    vector<Material*> GetAllMaterials();

private:
    vector<shared_ptr<Skeletal>> Bones;
    vector<shared_ptr<SkeletalMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;

    Matrix Transforms[MAX_MODEL_TRANSFORM];

    friend class Converter;
};
 