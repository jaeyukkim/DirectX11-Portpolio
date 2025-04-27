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
USkeletalMeshComponent::USkeletalMeshComponent(wstring InFileName)
{
	
	wstring objectName = InFileName;
	InFileName = L"../Contents/_Models/" + objectName + L".model";

	
	if (!filesystem::exists(InFileName))
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadFile(objectName + L"/" + objectName + L".fbx");
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
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 8 bytes
		{ "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
		{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
		{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 60,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 76,  D3D11_INPUT_PER_VERTEX_DATA, 0 }   // 16 bytes
	};


	
	for (const shared_ptr<SkeletalMesh>& meshPtr : SkeletalMeshes)
	{
		meshPtr->MaterialData->GetRenderer()->InitRenderer(inputElements, meshPtr->MaterialData->GetSamplerDesc());
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


void USkeletalMeshComponent::RenderComponent()
{
	Super::RenderComponent();

	for (const shared_ptr<SkeletalMesh>& meshPtr : SkeletalMeshes)
	{
		meshPtr->Render();
	}
}


void USkeletalMeshComponent::ReadFile(wstring InFileName)
{
	// 1. JSON 파일을 읽기 위한 입력 스트림 선언
	ifstream stream;

	// 2. 지정된 파일을 열기 (InFileName: 모델 데이터 파일)
	stream.open(InFileName);

	// 3. JSON 데이터 저장을 위한 Json::Value 객체 생성
	Json::Value root;

	// 4. JSON 파일에서 데이터를 읽어 `root` 객체에 저장
	stream >> root;
	
	// 5. JSON에서 파일 정보(머티리얼, 메시 파일명) 가져오기
	Json::Value material = root["File"]["Material"]; // "File" 섹션의 "Material" 필드
	Json::Value mesh = root["File"]["Mesh"]; // "File" 섹션의 "Mesh" 필드
	Json::Value transform = root["Transform"]; // "Transform" 섹션 (위치, 회전, 크기)

	// 6. 모델의 변환 정보(위치, 회전, 크기) 가져오기
	Json::Value position = transform["Position"]; // "Transform" 섹션의 "Position"
	Json::Value rotation = transform["Rotation"]; // "Transform" 섹션의 "Rotation"
	Json::Value scale = transform["Scale"];       // "Transform" 섹션의 "Scale"

	// 7. 머티리얼과 메시 파일명을 문자열로 변환
	string materialName = material.asString(); // 머티리얼 파일명
	string meshName = mesh.asString(); // 메시 파일명

	// 8. 변환된 데이터를 저장할 벡터 선언
	vector<string> pString;
	vector<string> sString;
	vector<string> rString;

	// 9. JSON 데이터(쉼표로 구분된 문자열)를 개별 숫자로 분할
	String::SplitString(&pString, position.asString(), ","); // 위치(Position) 데이터 분할
	String::SplitString(&sString, scale.asString(), ","); // 크기(Scale) 데이터 분할
	String::SplitString(&rString, rotation.asString(), ","); // 회전(Rotation) 데이터 분할

	// 10. 문자열 데이터를 float 값으로 변환하여 벡터(Vector)로 저장
	Vector3 p = Vector3(stof(pString[0]), stof(pString[1]), stof(pString[2])); // 위치
	Vector3 s = Vector3(stof(sString[0]), stof(sString[1]), stof(sString[2])); // 크기
	Vector3 r = Vector3(stof(rString[0]), stof(rString[1]), stof(rString[2])); // 회전

	// 11. 모델의 위치, 크기, 회전을 설정
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
	
	// 1. 머티리얼 파일(.material)의 경로를 설정
	InFilePath = L"../Contents/_Models/" + InFilePath  + L".material";

	// 2. 해당 파일이 위치한 폴더 경로 가져오기
	wstring textureFolder = Path::GetDirectoryName(InFilePath);

	// 3. JSON 파일을 읽기 위한 입력 스트림 생성
	ifstream stream;

	// 4. 파일을 열기
	stream.open(InFilePath);

	// 5. JSON 데이터를 저장할 객체 생성
	Json::Value root;

	// 6. JSON 파일에서 데이터를 읽어 `root` 객체에 저장
	stream >> root;

	// 7. JSON에서 모든 머티리얼(Key 값) 목록 가져오기
	Json::Value::Members members = root.getMemberNames();

	// 8. 각 머티리얼 정보를 순회하면서 읽어들임
	for (string name : members)
	{
		// 8.1 새로운 머티리얼 객체 생성
		shared_ptr<Material> material = make_shared<Material>();

		// 8.2 현재 머티리얼의 데이터 가져오기
		Json::Value value = root[name];

		// 8.3 셰이더(Shader) 이름이 존재하는 경우, 셰이더 설정
		if (value["VertexShaderPath"].asString().size() > 0)
			material->GetRenderer()->SetVertexShaderPath(String::ToWString(value["VertexShaderPath"].asString()));
		if (value["PixelShaderPath"].asString().size() > 0)
			material->GetRenderer()->SetPixelShaderPath(String::ToWString(value["PixelShaderPath"].asString()));

		
		// 8.4 머티리얼 색상 정보 설정 (Ambient, Diffuse, Specular, Emissive)
		material->SetAmbient(JsonStringToColor(value["Ambient"].asString()));
		material->SetDiffuse(JsonStringToColor(value["Diffuse"].asString()));
		material->SetSpecular(JsonStringToColor(value["Specular"].asString()));
		material->SetEmissive(JsonStringToColor(value["Emissive"].asString()));

		// 8.5 텍스처(DiffuseMap) 설정
		if (value["DiffuseMap"].asString().size() > 0)
			material->SetDiffuseMap(String::ToWString(value["DiffuseMap"].asString()));

		// 8.6 텍스처(SpecularMap) 설정
		if (value["SpecularMap"].asString().size() > 0)
			material->SetSpecularMap(String::ToWString(value["SpecularMap"].asString()));

		// 8.7 텍스처(NormalMap) 설정
		if (value["NormalMap"].asString().size() > 0)
			material->SetNormalMap(String::ToWString(value["NormalMap"].asString()));

		// 8.8 읽은 머티리얼을 MaterialTable에 저장 (name을 Key로 사용)
		MaterialTable[name] = material;
	}

	// 9. 파일 스트림을 닫음
	stream.close();
}


void USkeletalMeshComponent::ReadMesh(wstring InFilePath)
{
	// 1. 파일 경로를 설정 (모델 데이터가 저장된 경로)
	InFilePath = L"../Contents/_Models/" + InFilePath  + L".mesh";
	
	
	// 2. 이진 파일을 읽기 위한 BinaryReader 객체 생성
	unique_ptr<BinaryReader> reader = make_unique<BinaryReader>();

	// 3. 지정된 .mesh 파일을 열기
	reader->Open(InFilePath);

	
	// 4. 뼈대(Bone) 데이터를 읽어서 `Bones` 벡터에 저장
	Skeletal::ReadFile(reader.get(), Bones);
	

	// 5. 메시(Mesh) 데이터를 읽어서 `Meshes` 벡터 및 머티리얼(MaterialTable)에 저장
	SkeletalMesh::ReadFile(reader.get(), MaterialTable, SkeletalMeshes);

	// 6. 파일 닫기 및 메모리 정리
	reader->Close();
	

	// 7. Transform 행렬을 저장할 인덱스 초기화
	int count = 0;

	// 8. 각 Bone을 순회하며 Transform 정보를 저장하고, Bone과 Mesh를 연결
	for (shared_ptr<Skeletal>& bone : Bones)
	{
		// 8.1 뼈대의 Transform 행렬을 `Transforms` 배열에 저장
		Transforms[count++] = bone->Transform;

		// 8.2 해당 Bone이 관리하는 Mesh 리스트를 순회
		for (UINT number : bone->MeshNumbers)
		{
			// 8.3 현재 Bone의 Index를 Mesh의 BoneIndex로 설정
			SkeletalMeshes[number]->BoneIndex = bone->Index;

			// 8.4 Mesh가 참조하는 Bone을 설정
			SkeletalMeshes[number]->Bone = bone.get();

			// 8.5 Mesh가 참조하는 Transform 행렬 배열을 설정
			SkeletalMeshes[number]->Transforms = Transforms;
		}
	}
}


Color USkeletalMeshComponent::JsonStringToColor(string InString)
{
	vector<string> v;
	String::SplitString(&v, InString, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}