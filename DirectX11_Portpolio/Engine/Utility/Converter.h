#pragma once

#include <Assimp/Importer.hpp>
#include <Assimp/PostProcess.h>
#include <Assimp/Scene.h>

enum class EMeshType
{
    StaticMeshType,
    SkeletalMeshType
};

class Converter
{
public:
    Converter();
    ~Converter();

    void ReadFile(const wstring InFileName, bool InbIsGLTF);

public:
    void ExportMaterial(wstring InSaveFileName, bool InOverwrite, EMeshType InMeshType);


    string ColorToJson(const Color& InColor);
    string FloatToJson(const float val);

private:
    void ReadMaterials(EMeshType InMeshType);
    void WriteMaterial(wstring InSaveFileName, bool InOverwrite);
    string SaveTexture(string InSaveFolder, string InFileName);

public:
    void ExportMesh(wstring InSaveFileName, EMeshType FileType);
    
    
private:
    void ReadBoneData(aiNode* InNode, int InIndex, int InParent);
    void ReadSkeletalMeshData();
    void WriteSkeletalMeshData(wstring InSaveFileName);

private:
    void ReadStaticMeshData();
    void WriteStaticMeshData(wstring InSaveFileName);

private:
    string GetPixelShaderFileName(EMeshType InMeshType);
    string GetVertexShaderFileName(EMeshType InMeshType);

private:
    void ConvertToDXCoord(Vector3* normal, Vector3* tangent);

private:
    wstring ReadFilePath;

    shared_ptr<Assimp::Importer> Loader;
    const aiScene* Scene;

    vector<struct MaterialData*> Materials;
    vector<struct BoneData*> Bones;
    vector<struct SkeletalMeshData*> SkeletalMeshes;
    vector<struct StaticMeshData*> StaticMeshes;

private:
    bool bIsGLTF = false;
};