#pragma once
#include "VertexData.h"



struct MaterialData
{
    string Name;
    string VertexShaderPath;
    string PixelShaderPath;

    Color Ambient;
    Color Diffuse;
    Color Specular;
    Color Emissive;

    string DiffuseFile;
    string SpecularFile;
    string NormalFile;
};

struct BoneData
{
    UINT Index;
    string Name;

    int Parent;
    Matrix Transform;

    vector<UINT> MeshNumbers;
};

struct SkeletalMeshData
{
    string Name;

    string MaterialName;

    vector<VertexModel> Vertices;
    vector<UINT> Indices;
};

struct StaticMeshData
{
    string Name;

    string MaterialName;

    vector<VertexObject> Vertices;
    vector<UINT> Indices;
};