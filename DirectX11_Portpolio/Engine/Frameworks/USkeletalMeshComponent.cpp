#include "HeaderCollection.h"
#include "USkeletalMeshComponent.h"

#include <filesystem>
#include <fstream>
#include <json/reader.h>
#include <json/value.h>

#include "Utility/Converter.h"


/**
 * @param InFileName SkeletalMesh의 바이너리 파일 이름
 */
USkeletalMeshComponent::USkeletalMeshComponent(wstring InFileName, bool bOverwrite)
{
	wstring objectName = InFileName;
	InFileName = L"../Contents/_Models/" + objectName + L".model";
	
	if (bOverwrite)
	{
		wstring fbxPath = L"../Contents/_Assets/" + objectName + L"/" + objectName + L".fbx";
		wstring gltfPath = L"../Contents/_Assets/" + objectName + L"/" + objectName + L".gltf";

		wstring finalPath;
		if (filesystem::exists(fbxPath))
		{
			finalPath = fbxPath;
		}
		else if (filesystem::exists(gltfPath))
		{
			finalPath = gltfPath;
		}
		else
		{
			wcout << L"모델 파일이 존재하지 않습니다: " << endl;
			return;
		}

		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadFile(finalPath);
		converter->ExportMaterial(objectName, true,  EMeshType::SkeletalMeshType);
		converter->ExportMesh(objectName, EMeshType::SkeletalMeshType);
	}
	ReadFile(InFileName);
	InitRenderer();
}


/**
 * 반드시 렌더링 이전에 실행되어야 하는 함수
 */
void USkeletalMeshComponent::InitRenderer() const
{
	for (const shared_ptr<SkeletalMesh>& meshPtr : SkeletalMeshes)
	{
		meshPtr->MaterialData->GetRenderer()->InitRenderer(InputElementCollection::SkeletalMeshInputElement, 
																								meshPtr->MaterialData->GetSamplerDesc());
	}
}



void USkeletalMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	for (const shared_ptr<SkeletalMesh>& meshPtr : SkeletalMeshes)
	{
		meshPtr->SetWorld(GetWorldTransform());
		meshPtr->Tick();
	}
}


void USkeletalMeshComponent::RenderComponent(bool bUsePreRender)
{
	Super::RenderComponent(bUsePreRender);

	for (const shared_ptr<SkeletalMesh>& meshPtr : SkeletalMeshes)
	{
		meshPtr->Render(bUsePreRender);
	}
}


void USkeletalMeshComponent::ReadFile(wstring InFileName)
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

	string materialName = material.asString(); // 머티리얼 파일명
	string meshName = mesh.asString(); // 메시 파일명

	vector<string> pString;
	vector<string> sString;
	vector<string> rString;

	String::SplitString(&pString, position.asString(), ","); // 위치(Position) 데이터 분할
	String::SplitString(&sString, scale.asString(), ","); // 크기(Scale) 데이터 분할
	String::SplitString(&rString, rotation.asString(), ","); // 회전(Rotation) 데이터 분할

	Vector3 p = Vector3(stof(pString[0]), stof(pString[1]), stof(pString[2])); // 위치
	Vector3 s = Vector3(stof(sString[0]), stof(sString[1]), stof(sString[2])); // 크기
	Vector3 r = Vector3(stof(rString[0]), stof(rString[1]), stof(rString[2])); // 회전

	GetRelativeTransform()->SetPosition(p);
	GetRelativeTransform()->SetScale(s);
	GetRelativeTransform()->SetRotation(r.x, r.y, r.z);

	stream.close();

	// 12. 머티리얼과 메시 데이터를 로드
	ReadMaterial(String::ToWString(material.asString())); // 머티리얼 로드
	ReadMesh(String::ToWString(mesh.asString())); // 메시 로드
	
}


void USkeletalMeshComponent::ReadMaterial(wstring InFilePath)
{
	InFilePath = L"../Contents/_Models/" + InFilePath  + L".material";

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
		
		if (value["VertexShaderPath"].asString().size() > 0)
			material->GetRenderer()->SetVertexShaderPath(String::ToWString(value["VertexShaderPath"].asString()));
		if (value["PixelShaderPath"].asString().size() > 0)
			material->GetRenderer()->SetPixelShaderPath(String::ToWString(value["PixelShaderPath"].asString()));
		
		material->SetAlbedo(JsonStringToColor(value["Albedo"].asString()));
		material->SetRoughness(stof(value["Roughness"].asString()));
		material->SetMetallic(stof(value["Metallic"].asString()));
		material->SetEmissive(JsonStringToColor(value["Emissive"].asString()));
		
		
		if (value["AlbedoMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["AlbedoMap"].asString()), MaterialMapType::ALBEDO);
		
		if (value["MetallicMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["MetallicMap"].asString()), MaterialMapType::METALLIC);
		
		if (value["DiffuseRoughnessMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["DiffuseRoughnessMap"].asString()), MaterialMapType::DIFFUSE_ROUGHNESS);
		
		if (value["NormalMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["NormalMap"].asString()), MaterialMapType::NORMAL);
		
		if (value["AmbientOcclusionMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["AmbientOcclusionMap"].asString()), MaterialMapType::AMBIENTOCCLUSION);
		
		if (value["EmissiveMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["EmissiveMap"].asString()), MaterialMapType::EMISSIVE);

		if (value["HeightMap"].asString().size() > 0)
			material->SetTextureMap(String::ToWString(value["HeightMap"].asString()), MaterialMapType::HEIGHT);
		
		MaterialTable[name] = material;
	}
	
	stream.close();
}


void USkeletalMeshComponent::ReadMesh(wstring InFilePath)
{
	InFilePath = L"../Contents/_Models/" + InFilePath  + L".mesh";
	
	unique_ptr<BinaryReader> reader = make_unique<BinaryReader>();
	reader->Open(InFilePath);
	Skeletal::ReadFile(reader.get(), Bones);
	SkeletalMesh::ReadFile(reader.get(), MaterialTable, SkeletalMeshes);
	reader->Close();
	int count = 0;
	
	for (shared_ptr<Skeletal>& bone : Bones)
	{
		Transforms[count++] = bone->Transform;
		
		for (UINT number : bone->MeshNumbers)
		{
			SkeletalMeshes[number]->BoneIndex = bone->Index;
			SkeletalMeshes[number]->Bone = bone.get();
			SkeletalMeshes[number]->Transforms = Transforms;
		}
	}

	for (UINT i = 0; i < SkeletalMeshes.size(); i++)
	{
		if (SkeletalMeshes[i]->Transforms == nullptr)
		{
			assert(false);
		}
	}
}


Color USkeletalMeshComponent::JsonStringToColor(string InString)
{
	vector<string> v;
	String::SplitString(&v, InString, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}