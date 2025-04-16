#pragma once
#include "EPrimitiveComponent.h"
#include "Render/RenderData.h"

class EMeshComponent : public EPrimitiveComponent
{
public:

    vector<MeshData> MeshData;
};

