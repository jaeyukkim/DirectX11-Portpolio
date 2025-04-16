#pragma once
#include "EMeshComponent.h"
#include "Material.h"
#include "Skeletal.h"
#include "SkeletalMesh.h"


class ESkeletalMeshComponent : public EMeshComponent
{
public:
    ESkeletalMeshComponent(wstring InFileName);
    ~ESkeletalMeshComponent();

    void TickComponent();
    void InitRenderer() const;
    void Render();
    
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

private:
    shared_ptr<FTransform> MeshWorld;
};
 