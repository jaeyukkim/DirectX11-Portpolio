#pragma once
#include "BinaryFile.h"
#include "Frameworks/Components/USkeletalMeshComponent.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "Render/Mesh/Skeletal.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/Mesh/StaticMesh.h"

class FClipData;
class SkeletalMesh;
struct aiScene;

namespace Assimp
{
	class Importer;
}

struct aiNode;
class USkeletalMeshComponent;
class BinaryReader;
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
	void ReadFile(const wstring objectName, const EMeshType& meshType);

public:
	template<typename MeshType>
	void ReadMeshInfo(wstring InFileName, MeshType InMesh, bool bHasCreated);
	shared_ptr<FClipData> ReadAnimationData(aiAnimation* InAnimation);
	
public:
	void ExportMesh(wstring InSaveFileName, EMeshType FileType);
	void ExportMaterial(wstring InSaveFileName, bool InOverwrite, EMeshType InMeshType);
	void ExportAnimation(wstring objectName, wstring animationName, int InClipIndex = 0);
	
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

	void WriteAnimationData(wstring InSaveFileName, shared_ptr<FClipData> InClipData);
private:
	template<typename MeshType>
  void InitMaterial(wstring InFilePath, MeshType InMesh);
    
	template<typename MeshType>
	void InitMesh(wstring InFilePath, MeshType InMesh);
    
	template<typename MeshType>
	void ReadMeshData(BinaryReader* InReader, MeshType InMesh);
	void ReadBoneData(BinaryReader* InReader, USkeletalMeshComponent* meshComp);

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
	bool bIsGLTF = false;;
};


template <typename MeshType>
void Converter::ReadMeshInfo(wstring InFileName, MeshType InMesh, bool bHasCreated)
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

	// Renderer에 RenderProxy가 아직 생성되지 않았을 경우. 생성되었다면 pass
	if(!bHasCreated)
	{
		InitMaterial(String::ToWString(materialName), InMesh);
		InitMesh(String::ToWString(meshName), InMesh);
	}
    
}

template <typename MeshType>
void Converter::InitMaterial(wstring InFilePath, MeshType InMesh)
{
	UStaticMeshComponent* staticMeshComp = nullptr;
	USkeletalMeshComponent* skeletalMeshComp = nullptr;

    if (staticMeshComp = dynamic_cast<UStaticMeshComponent*>(InMesh))
	{
		InFilePath = L"../../Contents/_Objects/" + InFilePath + L".material";
	}
	else if (skeletalMeshComp = dynamic_cast<USkeletalMeshComponent*>(InMesh))
	{
		InFilePath = L"../../Contents/_Models/" + InFilePath + L".material";
	}
	else
	{
		Assert(true, "InitMaterial 실패")
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
			material->SetTextureMap(String::ToWString(value["AlbedoMap"].asString()), EMaterialMapType::ALBEDO);
		
		if (value["MetallicMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["MetallicMap"].asString()), EMaterialMapType::METALLIC);
		
		if (value["RoughnessMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["RoughnessMap"].asString()), EMaterialMapType::ROUGHNESS);
		
		if (value["NormalMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["NormalMap"].asString()), EMaterialMapType::NORMAL);
		
		if (value["AmbientOcclusionMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["AmbientOcclusionMap"].asString()), EMaterialMapType::AMBIENTOCCLUSION);
		
		if (value["EmissiveMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["EmissiveMap"].asString()), EMaterialMapType::EMISSIVE);

		if (value["HeightMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["HeightMap"].asString()), EMaterialMapType::HEIGHT);
		
		if(staticMeshComp != nullptr)
			staticMeshComp->MaterialTable[name] = material;
		else
			skeletalMeshComp->MaterialTable[name] = material;
	}
	
	stream.close();
}

template <typename MeshType>
void Converter::InitMesh(wstring InFilePath, MeshType InMesh)
{
	UStaticMeshComponent* staticMeshComp = nullptr;
	USkeletalMeshComponent* skeletalMeshComp = nullptr;
	if (staticMeshComp = dynamic_cast<UStaticMeshComponent*>(InMesh))
	{
		InFilePath = L"../../Contents/_Objects/" + InFilePath + L".mesh";
	}
	else if (skeletalMeshComp = dynamic_cast<USkeletalMeshComponent*>(InMesh))
	{
		InFilePath = L"../../Contents/_Models/" + InFilePath + L".mesh";
	}
	else
	{
		Assert(true, "InitMesh 실패")
	}
	
	unique_ptr<BinaryReader> reader = make_unique<BinaryReader>();
	reader->Open(InFilePath);
	ReadMeshData(reader.get(), InMesh);
	reader->Close();

	//스켈레탈 메시 타입이 아니라면 종료
	if(skeletalMeshComp == nullptr)
		return;
	
	int count = 0;

	vector<shared_ptr<SkeletalMesh>>& mesh = skeletalMeshComp->m_Mesh;
	for (shared_ptr<Skeletal>& bone : skeletalMeshComp->Bones)
	{
		skeletalMeshComp->Transforms[count++] = bone->Transform;
		
		for (UINT number : bone->MeshNumbers)
		{
			mesh[number]->Data.BoneIndex = bone->Index;
			mesh[number]->Data.Bone = bone.get();
			mesh[number]->Data.Transforms = skeletalMeshComp->Transforms;
		}
	}

	for (UINT i = 0; i < SkeletalMeshes.size(); i++)
	{
		if (mesh[i]->Data.Transforms == nullptr)
		{
			Assert(true, "SkeletalMesh의 Transform 정보가 없습니다.");
		}
	}
		
	
		
}

template <typename MeshType>
void Converter::ReadMeshData(BinaryReader* InReader, MeshType InMesh)
{
	

	if (auto staticMeshComp = dynamic_cast<UStaticMeshComponent*>(InMesh))
	{
		UINT count = InReader->FromUInt();
		
		for (UINT i = 0; i < count; i++)
		{
			auto mesh = make_shared<StaticMesh>();
			

			mesh->Data.Name = InReader->FromString();
			string materialName = InReader->FromString();
			mesh->Data.MaterialData = staticMeshComp->MaterialTable.at(materialName).get();
			mesh->Data.VertexCount = InReader->FromUInt();
			mesh->Data.Vertices = new VertexObject[mesh->Data.VertexCount];
			InReader->FromByte(mesh->Data.Vertices, sizeof(VertexObject) * mesh->Data.VertexCount);
		
			mesh->Data.IndexCount = InReader->FromUInt();
			mesh->Data.Indices = new UINT[mesh->Data.IndexCount];
			InReader->FromByte(mesh->Data.Indices, sizeof(UINT) * mesh->Data.IndexCount);
			InReader->FromByte(&mesh->Data.AABB.Max, sizeof(Vector3));
			InReader->FromByte(&mesh->Data.AABB.Min, sizeof(Vector3));
			staticMeshComp->m_Mesh.push_back(mesh);
		}
		for (shared_ptr<StaticMesh>& mesh : staticMeshComp->m_Mesh)
			mesh->CreateBuffer();
	}
	
	else if (auto skeletalMeshComp = dynamic_cast<USkeletalMeshComponent*>(InMesh))
	{
		ReadBoneData(InReader, skeletalMeshComp);
		
		UINT count = InReader->FromUInt();
		for (UINT i = 0; i < count; i++)
		{
			auto mesh = make_shared<SkeletalMesh>();
			auto& meshData = mesh->Data;

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
			
			InReader->FromByte(&mesh->Data.AABB.Max, sizeof(Vector3));
			InReader->FromByte(&mesh->Data.AABB.Min, sizeof(Vector3));
			skeletalMeshComp->m_Mesh.push_back(mesh);
		}

		for (shared_ptr<SkeletalMesh>& mesh : skeletalMeshComp->m_Mesh)
			mesh->CreateBuffer();
		
	}
	else
	{
		Assert(true, "ReadMeshData 실패");
	}
	
}
