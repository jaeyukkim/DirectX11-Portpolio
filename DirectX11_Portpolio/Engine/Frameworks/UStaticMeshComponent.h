#pragma once
#include "UPrimitiveComponent.h"
#include "Render/RenderData.h"

class StaticMesh;
class Material;

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    UStaticMeshComponent(wstring InFileName, bool bOverwirte = false);
    virtual ~UStaticMeshComponent() = default;

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent(bool bUsePreRender = false) override;
    virtual void DrawComponentIndex();
public:
    void InitRenderer();
    void ReverseIndices();

    Material* GetMaterial(string InMaterialName);
    
private:

    void ReadFile(wstring InFileName);
    void ReadMaterial(wstring InFilePath);
    void ReadMesh(wstring InFilePath);

    Color JsonStringToColor(string InString);

private:
    vector<shared_ptr<StaticMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;

private:
    mutable bool bInitRenderComplete = false;
};

