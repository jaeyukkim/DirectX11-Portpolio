#include "HeaderCollection.h"
#include "GeometryGenerator.h"


/*
* 사각형을 만드는 함수
*/
StaticMeshData GeometryGenerator::MakeSquare(const float scale, const Vector2 texScale) 
{
    vector<Vector3> positions;
    vector<Vector3> colors;
    vector<Vector3> normals;
    vector<Vector2> Uvs; 

    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

  
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    StaticMeshData meshData;

    for (size_t i = 0; i < positions.size(); i++)
    {
        VertexObject v;
        v.Position = positions[i];
        v.Normal = normals[i];
        v.Uv = Uvs[i] * texScale;
        v.Tangent = Vector3(1.0f, 0.0f, 0.0f);

        meshData.Vertices.push_back(v);
    }

    meshData.Indices = {0, 1, 2, 0, 2, 3};
    return meshData;
}


/**
* 사각형 그리드를 생성하는 함수
*/
StaticMeshData GeometryGenerator::MakeSquareGrid(const int numSlices, const int numStacks,
    const float scale, const Vector2 texScale) 
{
    StaticMeshData meshData;

    float dx = 2.0f / numSlices;
    float dy = 2.0f / numStacks;

    float y = 1.0f;
    for (int j = 0; j < numStacks + 1; j++) 
    {
        float x = -1.0f;
        for (int i = 0; i < numSlices + 1; i++)
        {
            VertexObject v;
            v.Position = Vector3(x, y, 0.0f) * scale;
            v.Normal = Vector3(0.0f, 0.0f, -1.0f);
            v.Uv = Vector2(x + 1.0f, y + 1.0f) * 0.5f * texScale;
            v.Tangent = Vector3(1.0f, 0.0f, 0.0f);

            meshData.Vertices.push_back(v);

            x += dx;
        }
        y -= dy;
    }

    for (int j = 0; j < numStacks; j++) 
    {
        for (int i = 0; i < numSlices; i++) 
        {
            meshData.Indices.push_back((numSlices + 1) * j + i);
            meshData.Indices.push_back((numSlices + 1) * j + i + 1);
            meshData.Indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.Indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.Indices.push_back((numSlices + 1) * j + i + 1);
            meshData.Indices.push_back((numSlices + 1) * (j + 1) + i + 1);
        }
    }

    return meshData;
}


/**
* 박스를 생성하는 함수
*/
StaticMeshData GeometryGenerator::MakeBox(const float scale) 
{

    vector<Vector3> positions;
    vector<Vector3> colors;
    vector<Vector3> normals;
    vector<Vector2> Uvs; // 텍스춰 좌표

    // 윗면
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    // 아랫면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    // 앞면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    // 뒷면
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    // 왼쪽
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    // 오른쪽
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    Uvs.push_back(Vector2(0.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 0.0f));
    Uvs.push_back(Vector2(1.0f, 1.0f));
    Uvs.push_back(Vector2(0.0f, 1.0f));

    StaticMeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) 
    {
        VertexObject v;
        v.Position = positions[i];
        v.Normal = normals[i];
        v.Uv = Uvs[i];
        meshData.Vertices.push_back(v);
    }

    meshData.Indices = 
    {
        0,  1,  2,  0,  2,  3,  // 윗면
        4,  5,  6,  4,  6,  7,  // 아랫면
        8,  9,  10, 8,  10, 11, // 앞면
        12, 13, 14, 12, 14, 15, // 뒷면
        16, 17, 18, 16, 18, 19, // 왼쪽
        20, 21, 22, 20, 22, 23  // 오른쪽
    };

    return meshData;
}

StaticMeshData GeometryGenerator::MakeCylinder(const float bottomRadius,
    const float topRadius, float height, int numSlices) 
{

    // Texture 좌표계때문에 (numSlices + 1) x 2 개의 버텍스 사용
    const float dTheta = -XM_2PI / float(numSlices);
    StaticMeshData meshData;
    vector<VertexObject>& Vertices = meshData.Vertices;

    // 옆면의 바닥 버텍스들 (인덱스 0 이상 numSlices 미만)
    for (int i = 0; i <= numSlices; i++) 
    {
        VertexObject v;

        v.Position = Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f), 
                            Matrix::CreateRotationY(dTheta * float(i)));
        v.Normal = v.Position - Vector3(0.0f, -0.5f * height, 0.0f);
        v.Normal.Normalize();
        v.Uv = Vector2(float(i) / numSlices, 1.0f);
        Vertices.push_back(v);
    }

    // 옆면의 맨 위 버텍스들 (인덱스 numSlices 이상 2 * numSlices 미만)
    for (int i = 0; i <= numSlices; i++) 
    {
        VertexObject v;

        v.Position = Vector3::Transform(Vector3(topRadius, 0.5f * height, 0.0f),
                            Matrix::CreateRotationY(dTheta * float(i)));
        v.Normal = v.Position - Vector3(0.0f, 0.5f * height, 0.0f);
        v.Normal.Normalize();
        v.Uv = Vector2(float(i) / numSlices, 0.0f);

        Vertices.push_back(v);
    }

    vector<uint32_t>& Indices = meshData.Indices;

    for (int i = 0; i < numSlices; i++) 
    {
        Indices.push_back(i);
        Indices.push_back(i + numSlices + 1);
        Indices.push_back(i + 1 + numSlices + 1);

        Indices.push_back(i);
        Indices.push_back(i + 1 + numSlices + 1);
        Indices.push_back(i + 1);
    }

    return meshData;
}

StaticMeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices,
    const int numStacks,
    const Vector2 texScale) 
{
    const float dTheta = -XM_2PI / float(numSlices);
    const float dPhi = -XM_PI / float(numStacks);

    StaticMeshData meshData;
    vector<VertexObject>& Vertices = meshData.Vertices;

    for (int j = 0; j <= numStacks; j++) 
    {
        // 스택에 쌓일 수록 시작점을 x-y 평면에서 회전 시켜서 위로 올리는 구조
        Vector3 stackStartPoint = Vector3::Transform(Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * j));

        for (int i = 0; i <= numSlices; i++) 
        {
            VertexObject v;

            // 시작점을 x-z 평면에서 회전시키면서 원을 만드는 구조
            v.Position = Vector3::Transform(stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));
            v.Normal = v.Position; // 원점이 구의 중심
            v.Normal.Normalize();
            v.Uv = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks) * texScale;

            // Uv가 위로 갈수록 증가
            Vector3 biTangent = Vector3(0.0f, 1.0f, 0.0f);

            Vector3 normalOrth = v.Normal - biTangent.Dot(v.Normal) * v.Normal;
            normalOrth.Normalize();
            v.Tangent = biTangent.Cross(normalOrth);
            v.Tangent.Normalize();

            Vertices.push_back(v);
        }
    }


    vector<uint32_t>& Indices = meshData.Indices;

    for (int j = 0; j < numStacks; j++) 
    {
        const int offset = (numSlices + 1) * j;

        for (int i = 0; i < numSlices; i++) 
        {
            Indices.push_back(offset + i);
            Indices.push_back(offset + i + numSlices + 1);
            Indices.push_back(offset + i + 1 + numSlices + 1);

            Indices.push_back(offset + i);
            Indices.push_back(offset + i + 1 + numSlices + 1);
            Indices.push_back(offset + i + 1);
        }
    }


    return meshData;
}

StaticMeshData GeometryGenerator::MakeIcosahedron() 
{

    const float X = 0.525731f;
    const float Z = 0.850651f;

    StaticMeshData newMesh;

    vector<Vector3> pos = 
    {
        Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),   Vector3(-X, 0.0f, -Z),
        Vector3(X, 0.0f, -Z), Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
        Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X), Vector3(Z, X, 0.0f),
        Vector3(-Z, X, 0.0f), Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f) 
    };

    for (size_t i = 0; i < pos.size(); i++) 
    {
        VertexObject v;
        v.Position = pos[i];
        v.Normal = v.Position;
        v.Normal.Normalize();

        newMesh.Vertices.push_back(v);
    }

    newMesh.Indices = 
    { 1,  4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1,  8, 4,
      1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5,  3,  7, 2,
      3,  10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
      10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9,  11, 2, 7 };

    return newMesh;
}

StaticMeshData GeometryGenerator::MakeTetrahedron() 
{

    const float a = 1.0f;
    const float x = sqrt(3.0f) / 3.0f * a;
    const float d = sqrt(3.0f) / 6.0f * a; // = x / 2
    const float h = sqrt(6.0f) / 3.0f * a;

    vector<Vector3> points = { {0.0f, x, 0.0f},
                              {-0.5f * a, -d, 0.0f},
                              {+0.5f * a, -d, 0.0f},
                              {0.0f, 0.0f, h} };

    Vector3 center = Vector3(0.0f);

    for (int i = 0; i < 4; i++) 
    {
        center += points[i];
    }
    center /= 4.0f;

    for (int i = 0; i < 4; i++) 
    {
        points[i] -= center;
    }

    StaticMeshData meshData;

    for (int i = 0; i < points.size(); i++) 
    {

        VertexObject v;
        v.Position = points[i];
        v.Normal = v.Position; // 중심이 원점
        v.Normal.Normalize();

        meshData.Vertices.push_back(v);
    }

    meshData.Indices = { 0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3 };

    return meshData;
}
StaticMeshData GeometryGenerator::SubdivideToSphere(const float radius,
    StaticMeshData meshData) 
{

    using namespace DirectX;
    using DirectX::SimpleMath::Matrix;
    using DirectX::SimpleMath::Vector3;

    // 원점이 중심이라고 가정
    for (auto& v : meshData.Vertices) 
    {
        v.Position = v.Normal * radius;
    }

    // 구의 표면으로 옮기고 노멀과 texture 좌표 계산
    auto ProjectVertexObject = [&](VertexObject& v) 
    {
        v.Normal = v.Position;
        v.Normal.Normalize();
        v.Position = v.Normal * radius;
    };


    auto UpdateFaceNormal = [](VertexObject& v0, VertexObject& v1, VertexObject& v2) 
    {
        auto faceNormal = (v1.Position - v0.Position).Cross(v2.Position - v0.Position);
        faceNormal.Normalize();
        v0.Normal = faceNormal;
        v1.Normal = faceNormal;
        v2.Normal = faceNormal;
    };

    // 버텍스가 중복되는 구조로 구현
    StaticMeshData newMesh;
    uint32_t count = 0;

    for (size_t i = 0; i < meshData.Indices.size(); i += 3) 
    {
        size_t i0 = meshData.Indices[i];
        size_t i1 = meshData.Indices[i + 1];
        size_t i2 = meshData.Indices[i + 2];

        VertexObject v0 = meshData.Vertices[i0];
        VertexObject v1 = meshData.Vertices[i1];
        VertexObject v2 = meshData.Vertices[i2];

        VertexObject v3;
        v3.Position = (v0.Position + v2.Position) * 0.5f;
        v3.Uv = (v0.Uv + v2.Uv) * 0.5f;
        ProjectVertexObject(v3);

        VertexObject v4;
        v4.Position = (v0.Position + v1.Position) * 0.5f;
        v4.Uv = (v0.Uv + v1.Uv) * 0.5f;
        ProjectVertexObject(v4);

        VertexObject v5;
        v5.Position = (v1.Position + v2.Position) * 0.5f;
        v5.Uv = (v1.Uv + v2.Uv) * 0.5f;
        ProjectVertexObject(v5);

        newMesh.Vertices.push_back(v4);
        newMesh.Vertices.push_back(v1);
        newMesh.Vertices.push_back(v5);

        newMesh.Vertices.push_back(v0);
        newMesh.Vertices.push_back(v4);
        newMesh.Vertices.push_back(v3);

        newMesh.Vertices.push_back(v3);
        newMesh.Vertices.push_back(v4);
        newMesh.Vertices.push_back(v5);

        newMesh.Vertices.push_back(v3);
        newMesh.Vertices.push_back(v5);
        newMesh.Vertices.push_back(v2);

        for (uint32_t j = 0; j < 12; j++) 
        {
            newMesh.Indices.push_back(j + count);
        }
        count += 12;
    }

    return newMesh;
}