#include "HeaderCollection.h"
#include "UStaticMeshComponent.h"
#include "StaticMesh.h"
#include "Utility/BinaryFile.h"

#include <filesystem>
#include <fstream>
#include <json/reader.h>
#include <json/value.h>

#include "Utility/Converter.h"


/**
 * @param InFileName StaticMesh의 바이너리 파일 이름
 */
UStaticMeshComponent::UStaticMeshComponent(wstring InFileName, bool bOverwrite)
{

	wstring objectName = InFileName;
	InFileName = L"../Contents/_Objects/" + objectName + L".model";


	if (bOverwrite)
	{
		wstring fbxPath = L"../Contents/_Assets/" + objectName + L"/" + objectName + L".fbx";
		wstring gltfPath = L"../Contents/_Assets/" + objectName + L"/" + objectName + L".gltf";
		bool bIsGLTF = false;
		wstring finalPath;
		if (filesystem::exists(fbxPath))
		{
			finalPath = fbxPath;
		}
		else if (filesystem::exists(gltfPath))
		{
			finalPath = gltfPath;
			bIsGLTF = true;
		}
		else
		{
			wcout << L"모델 파일이 존재하지 않습니다: " << endl;
			return;
		}

		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadFile(finalPath, bIsGLTF);
		converter->ExportMaterial(objectName, true, EMeshType::StaticMeshType);
		converter->ExportMesh(objectName, EMeshType::StaticMeshType);
	}
	ReadFile(InFileName);
	InitRenderer();
}

/**
 * 렌더링 이전에 반드시 실행해야 하는 함수
 */
void UStaticMeshComponent::InitRenderer()
{
	
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->MaterialData->GetRenderer()->InitRenderer(InputElementCollection::basicInputElement, meshPtr->MaterialData->GetSamplerDesc());
		meshPtr->MaterialData->GetRenderer()->CreateSamplerState(ESamplerSlot::ClampSampler);
	}

	bInitRenderComplete = true;
}

/**
*  모든 메터리얼 반환
*/
vector<Material*> UStaticMeshComponent::GetAllMaterials()
{
	vector<Material*> result;

	for (auto& [name, matPtr] : MaterialTable)
	{
		if (matPtr)
			result.push_back(matPtr.get());
	}

	return result;
}

void UStaticMeshComponent::ReverseIndices()
{
	for (auto& mesh : m_Mesh)
	{
		for (UINT i = 0; i < mesh->IndexCount; i += 3)
		{
			std::swap(mesh->Indices[i + 1], mesh->Indices[i + 2]);
		}
		mesh->CreateBuffer();
	}

}


void UStaticMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);
	
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->SetWorld(GetWorldTransform());
		meshPtr->Tick();
	}
	
}

/**
 * InitRenderer 완료되었으면 호출 가능
 */
void UStaticMeshComponent::RenderComponent()
{
	Super::RenderComponent();

	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->Render();
	}
	
}

void UStaticMeshComponent::RenderMirror(const Matrix& refl)
{
	USceneComponent::RenderComponent();
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->RenderMirror(refl);

		Shader* renderer = meshPtr->GetMaterialData()->GetRenderer();

		// 거울 자체의 재질을 "Blend"로 그린 후 복구
		renderer->SetMirrorPipeline();
		FSceneView::Get()->PreRender();
		renderer->SetDefaultRasterizeState();
		USceneComponent::RenderComponent();
		meshPtr->Render();
		D3D::Get()->ClearBlendState();
		renderer->SetDefaultDepthStencilState();
	}
}

void UStaticMeshComponent::DrawComponentIndex()
{
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->DrawIndexed();
	}
}



void UStaticMeshComponent::ReadFile(wstring InFileName)
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


/**
 * @param InFilePath ".material" 파일의 저장 경로 \n
 * 메터리얼 파일을 읽어 MaterialTable에 저장
 */	
void UStaticMeshComponent::ReadMaterial(wstring InFilePath)
{
	
	InFilePath = L"../Contents/_Objects/" + InFilePath + L".material";

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
		
		MaterialTable[name] = material;
	}
	
	stream.close();
}


void UStaticMeshComponent::ReadMesh(wstring InFilePath)
{
	InFilePath = L"../Contents/_Objects/" + InFilePath + L".mesh";
	unique_ptr<BinaryReader> reader = make_unique<BinaryReader>();
	reader->Open(InFilePath);
	StaticMesh::ReadFile(reader.get(), MaterialTable, m_Mesh);
	reader->Close();
}


Color UStaticMeshComponent::JsonStringToColor(string InString)
{
	vector<string> v;
	String::SplitString(&v, InString, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}