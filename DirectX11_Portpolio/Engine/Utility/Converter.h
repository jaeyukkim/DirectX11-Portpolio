#pragma once

#include <Assimp/Importer.hpp>
#include <Assimp/PostProcess.h>
#include <Assimp/Scene.h>
#include <json/json.h>
#include <fstream>
#include "BinaryFile.h"

#include "Frameworks/USkeletalMeshComponent.h"
#include "Frameworks/UStaticMeshComponent.h"

class StaticMesh;
class String;


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
    void ReadFile(const wstring objectName);

public:
    template<typename MeshType>
    void ReadMeshInfo(wstring InFileName, MeshType* InMesh);

public:
    void ExportMesh(wstring InSaveFileName, EMeshType FileType);
    void ExportMaterial(wstring InSaveFileName, bool InOverwrite, EMeshType InMeshType);
    string ColorToJson(const Color& InColor);
    string FloatToJson(const float val);
    Color JsonStringToColor(string InString);
    

private:
    void ReadMaterials(EMeshType InMeshType);
    void WriteMaterial(wstring InSaveFileName, bool InOverwrite);
    string SaveTexture(string InSaveFolder, string InFileName);

    void ReadStaticMeshData();
    void WriteStaticMeshData(wstring InSaveFileName);
    
    void ReadBoneData(aiNode* InNode, int InIndex, int InParent);
    void ReadSkeletalMeshData();
    void WriteSkeletalMeshData(wstring InSaveFileName);

private:
	template<typename MeshType>
  void InitMaterial(wstring InFilePath, MeshType* InMesh);
    
	template<typename MeshType>
	void InitMesh(wstring InFilePath, MeshType* InMesh);
    
	template<typename MeshType>
	void ReadMeshData(BinaryReader* InReader, MeshType* InMesh);

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



template <typename MeshType>
void Converter::ReadMeshInfo(wstring InFileName, MeshType* InMesh)
{
    ifstream stream;
    stream.open(InFileName);
    Json::Value root;
    stream >> root;
	
    Json::Value material = root["File"]["Material"]; // "File" 섹션의 "Material" 필드
    Json::Value mesh = root["File"]["Mesh"]; // "File" 섹션의 "Mesh" 필드
    Json::Value transform = root["Transform"]; // "Transform" 섹션 (위치, 회전, 크기)
	
    Json::Value position = transform["Position"]; // "Transform" 섹션의 "Position"
    Json::Value rotation = transform["Rotation"]; // "Transform" 섹션의 "Rotation"
    Json::Value scale = transform["Scale"];       // "Transform" 섹션의 "Scale"
	
    string materialName = material.asString();
    string meshName = mesh.asString();
	
    vector<string> pString;
    vector<string> sString;
    vector<string> rString;
	
    String::SplitString(&pString, position.asString(), ","); // 위치(Position) 데이터 분할
    String::SplitString(&sString, scale.asString(), ","); // 크기(Scale) 데이터 분할
    String::SplitString(&rString, rotation.asString(), ","); // 회전(Rotation) 데이터 분할
	
    Vector3 p = Vector3(stof(pString[0]), stof(pString[1]), stof(pString[2])); // 위치
    Vector3 s = Vector3(stof(sString[0]), stof(sString[1]), stof(sString[2])); // 크기
    Vector3 r = Vector3(stof(rString[0]), stof(rString[1]), stof(rString[2])); // 회전

    InMesh->GetRelativeTransform()->SetPosition(p);
    InMesh->GetRelativeTransform()->SetScale(s);
    InMesh->GetRelativeTransform()->SetRotation(r.x, r.y, r.z);

    stream.close();
	
    InitMaterial(String::ToWString(materialName), InMesh);
    InitMesh(String::ToWString(meshName), InMesh);
}

template <typename MeshType>
void Converter::InitMaterial(wstring InFilePath, MeshType* InMesh)
{
    if (auto staticMeshComp = dynamic_cast<UStaticMeshComponent*>(InMesh))
	{
		InFilePath = L"../Contents/_Objects/" + InFilePath + L".material";
	}
	else if (auto skeletalMeshComp = dynamic_cast<USkeletalMeshComponent*>(InMesh))
	{
		InFilePath = L"../Contents/_Models/" + InFilePath + L".material";
	}
	
	wstring textureFolder = Path::GetDirectoryName(InFilePath);
	ifstream stream;
	stream.open(InFilePath);
	Json::Value root;
	stream >> root;
	Json::Value::Members members = root.getMemberNames();
	
	for (string name : members)
	{
		shared_ptr<Material> material = make_shared<Material>();
		Json::Value value = root[name];
		
		material->SetAlbedo(JsonStringToColor(value["Albedo"].asString()));
		material->SetRoughness(stof(value["Roughness"].asString()));
		material->SetMetallic(stof(value["Metallic"].asString()));
		material->SetEmissive(JsonStringToColor(value["Emissive"].asString()));
		
		if (value["AlbedoMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["AlbedoMap"].asString()), MaterialMapType::ALBEDO);
		
		if (value["MetallicMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["MetallicMap"].asString()), MaterialMapType::METALLIC);
		
		if (value["RoughnessMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["RoughnessMap"].asString()), MaterialMapType::ROUGHNESS);
		
		if (value["NormalMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["NormalMap"].asString()), MaterialMapType::NORMAL);
		
		if (value["AmbientOcclusionMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["AmbientOcclusionMap"].asString()), MaterialMapType::AMBIENTOCCLUSION);
		
		if (value["EmissiveMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["EmissiveMap"].asString()), MaterialMapType::EMISSIVE);

		if (value["HeightMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["HeightMap"].asString()), MaterialMapType::HEIGHT);
		
		InMesh->MaterialTable[name] = material;
	}
	
	stream.close();
}

template <typename MeshType>
void Converter::InitMesh(wstring InFilePath, MeshType* InMesh)
{
	if (auto staticMeshComp = dynamic_cast<UStaticMeshComponent*>(InMesh))
	{
		InFilePath = L"../Contents/_Objects/" + InFilePath + L".mesh";
	}
	else if (auto skeletalMeshComp = dynamic_cast<USkeletalMeshComponent*>(InMesh))
	{
		InFilePath = L"../Contents/_Models/" + InFilePath + L".mesh";
	}
	else
	{
		Assert(true, "InitMesh 실패")
	}
	
	unique_ptr<BinaryReader> reader = make_unique<BinaryReader>();
	reader->Open(InFilePath);
	ReadMeshData(reader.get(), InMesh);
	reader->Close();
}

template <typename MeshType>
void Converter::ReadMeshData(BinaryReader* InReader, MeshType* InMesh)
{
	UINT count = InReader->FromUInt();

	if (auto staticMeshComp = dynamic_cast<UStaticMeshComponent*>(InMesh))
	{
		for (UINT i = 0; i < count; i++)
		{
			auto mesh = make_shared<StaticMesh>();
			auto meshData = mesh->Data;

			meshData.Name = InReader->FromString();
			string materialName = InReader->FromString();
			meshData.MaterialData = staticMeshComp->MaterialTable.at(materialName).get();
			meshData.VertexCount = InReader->FromUInt();
			meshData.Vertices = new VertexObject[meshData.VertexCount];
			InReader->FromByte(meshData.Vertices, sizeof(VertexObject) * meshData.VertexCount);
		
			meshData.IndexCount = InReader->FromUInt();
			meshData.Indices = new UINT[meshData.IndexCount];
			InReader->FromByte(meshData.Indices, sizeof(UINT) * meshData.IndexCount);
			staticMeshComp->m_Mesh.push_back(mesh);
		}
		for (shared_ptr<StaticMesh>& mesh : staticMeshComp->m_Mesh)
			mesh->CreateBuffer();
	}
	
	else if (auto skeletalMeshComp = dynamic_cast<USkeletalMeshComponent*>(InMesh))
	{
		for (UINT i = 0; i < count; i++)
		{
			auto mesh = make_shared<SkeletalMesh>();
			auto meshData = mesh->Data;

			meshData.Name = InReader->FromString();
			string materialName = InReader->FromString();
			meshData.MaterialData = skeletalMeshComp->MaterialTable.at(materialName).get();

			mesh->BoneData.BoneIndex = InReader->FromUInt();

			meshData.VertexCount = InReader->FromUInt();
			meshData.ModelVertices = new VertexModel[meshData.VertexCount];
			InReader->FromByte(meshData.ModelVertices, sizeof(VertexModel) * meshData.VertexCount);

			meshData.IndexCount = InReader->FromUInt();
			meshData.Indices = new UINT[meshData.IndexCount];
			InReader->FromByte(meshData.Indices, sizeof(UINT) * meshData.IndexCount);

			skeletalMeshComp->m_Mesh.push_back(mesh);
		}

		for (shared_ptr<SkeletalMesh>& mesh : skeletalMeshComp->m_Mesh)
			mesh->CreateBuffer();
	}
	
}
