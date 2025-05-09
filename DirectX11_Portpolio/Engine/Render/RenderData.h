#pragma once
#include "VertexData.h"



struct MaterialData
{
    string Name;
    string VertexShaderPath;
    string PixelShaderPath;

    Color Albedo;
    float Roughness;
    float Metallic;
    Color Emissive;

    string AlbedoFile;
    string MetallicFile;
    string DiffuseRoughnessFile;
    string NormalFile;
    string AmbientOcclusionFile;
    string EmissiveFile;
    string HeightFile;
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
    int BoneIndex;

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