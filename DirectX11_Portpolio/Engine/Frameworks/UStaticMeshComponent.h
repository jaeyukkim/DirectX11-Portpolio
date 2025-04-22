#pragma once
#include "UPrimitiveComponent.h"
#include "Render/RenderData.h"

class StaticMesh;


class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    UStaticMeshComponent(wstring InFileName);
    virtual ~UStaticMeshComponent();

public:
    virtual void TickComponent(float deltaTime) override;
    virtual void RenderComponent() override;


    void InitRenderer() const;


private:

    void ReadFile(wstring InFileName);
    void ReadMaterial(wstring InFilePath);
    void ReadMesh(wstring InFilePath);

    Color JsonStringToColor(string InString);

private:

    vector<shared_ptr<StaticMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;

};

