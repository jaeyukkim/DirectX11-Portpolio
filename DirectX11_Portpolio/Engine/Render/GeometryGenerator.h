#pragma once


struct StaticMeshData;

class GeometryGenerator 
{
public:

    static StaticMeshData MakeSquare(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeSquareGrid(const int numSlices, const int numStacks,const float scale = 1.0f,
        const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeBox(const float scale = 1.0f);
    static StaticMeshData MakeCylinder(const float bottomRadius, const float topRadius, float height, int numSlices);
    static StaticMeshData MakeSphere(const float radius, const int numSlices, const int numStacks,
        const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeTetrahedron();
    static StaticMeshData MakeIcosahedron();
    static StaticMeshData SubdivideToSphere(const float radius, StaticMeshData meshData);
};