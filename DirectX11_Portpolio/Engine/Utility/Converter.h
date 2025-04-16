#pragma once

#include <Assimp/Importer.hpp>
#include <Assimp/PostProcess.h>
#include <Assimp/Scene.h>

class Converter
{
public:
    Converter();
    ~Converter();

    void ReadFile(const wstring InFileName);

public:
    void ExportMaterial(wstring InSaveFileName, bool InOverwrite = true);

private:
    void ReadMaterials();
    void WriteMaterial(wstring InSaveFileName, bool InOverwrite);

    string ColorToJson(const Color& InColor);
    string SaveTexture(string InSaveFolder, string InFileName);

public:
    void ExportMesh(wstring InSaveFileName);

private:
    void ReadBoneData(aiNode* InNode, int InIndex, int InParent);
    void ReadMeshData();

    void WriteMeshData(wstring InSaveFileName);

private:
    wstring ReadFilePath;

    shared_ptr<Assimp::Importer> Loader;
    const aiScene* Scene;

    vector<struct MaterialData*> Materials;
    vector<struct BoneData*> Bones;
    vector<struct MeshData*> Meshes;
};