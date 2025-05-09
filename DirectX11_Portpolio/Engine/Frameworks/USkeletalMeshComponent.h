#pragma once
#include "UPrimitiveComponent.h"
#include "Skeletal.h"
#include "SkeletalMesh.h"



class USkeletalMeshComponent : public UPrimitiveComponent
{
public:
    USkeletalMeshComponent(wstring InFileName, bool bOverwrite = false);
    virtual ~USkeletalMeshComponent() = default;

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent(bool bUsePreRender = false) override;

    void InitRenderer() const;
    
    
private:
    void ReadFile(wstring InFileName);
    void ReadMaterial(wstring InFilePath);
    void ReadMesh(wstring InFilePath);

    Color JsonStringToColor(string InString);
    
private:
    vector<shared_ptr<Skeletal>> Bones;
    vector<shared_ptr<SkeletalMesh>> SkeletalMeshes;
    map<string, shared_ptr<Material>> MaterialTable;

    Matrix Transforms[MAX_MODEL_TRANSFORM];

};
 