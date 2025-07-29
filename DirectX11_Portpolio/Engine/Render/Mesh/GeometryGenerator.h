#pragma once


struct StaticMeshData;

class GeometryGenerator 
{
public:

    static VertexTextureData MakeScreen(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeSquare(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeSquareGrid(const int numSlices, const int numStacks,const float scale = 1.0f,
        const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeBox(Vector3 scale);
    static StaticMeshData MakeCapsule(float radius, float halfHeight, int numSlices = 24, int numStacks = 12);
    
    static StaticMeshData MakeCylinder(const float bottomRadius, const float topRadius, float height, int numSlices);
    static StaticMeshData MakeSphere(const float radius, const int numSlices, const int numStacks,
        const Vector2 texScale = Vector2(1.0f));
    static StaticMeshData MakeTetrahedron();
    static StaticMeshData MakeIcosahedron();
    static StaticMeshData SubdivideToSphere(const float radius, StaticMeshData meshData);

    static void SetAABB(StaticMeshData& InMeshData);
};