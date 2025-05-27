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
 * @param InFileName StaticMesh�� ���̳ʸ� ���� �̸�
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
			wcout << L"�� ������ �������� �ʽ��ϴ�: " << endl;
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
 * ������ ������ �ݵ�� �����ؾ� �ϴ� �Լ�
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
*  ��� ���͸��� ��ȯ
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
 * InitRenderer �Ϸ�Ǿ����� ȣ�� ����
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

		// �ſ� ��ü�� ������ "Blend"�� �׸� �� ����
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
	// 1. JSON ������ �б� ���� �Է� ��Ʈ�� ����
	ifstream stream;

	// 2. ������ ������ ���� (InFileName: �� ������ ����)
	stream.open(InFileName);

	// 3. JSON ������ ������ ���� Json::Value ��ü ����
	Json::Value root;

	// 4. JSON ���Ͽ��� �����͸� �о� `root` ��ü�� ����
	stream >> root;
	
	// 5. JSON���� ���� ����(��Ƽ����, �޽� ���ϸ�) ��������
	Json::Value material = root["File"]["Material"]; // "File" ������ "Material" �ʵ�
	Json::Value mesh = root["File"]["Mesh"]; // "File" ������ "Mesh" �ʵ�
	Json::Value transform = root["Transform"]; // "Transform" ���� (��ġ, ȸ��, ũ��)

	// 6. ���� ��ȯ ����(��ġ, ȸ��, ũ��) ��������
	Json::Value position = transform["Position"]; // "Transform" ������ "Position"
	Json::Value rotation = transform["Rotation"]; // "Transform" ������ "Rotation"
	Json::Value scale = transform["Scale"];       // "Transform" ������ "Scale"

	// 7. ��Ƽ����� �޽� ���ϸ��� ���ڿ��� ��ȯ
	string materialName = material.asString(); // ��Ƽ���� ���ϸ�
	string meshName = mesh.asString(); // �޽� ���ϸ�

	// 8. ��ȯ�� �����͸� ������ ���� ����
	vector<string> pString;
	vector<string> sString;
	vector<string> rString;

	// 9. JSON ������(��ǥ�� ���е� ���ڿ�)�� ���� ���ڷ� ����
	String::SplitString(&pString, position.asString(), ","); // ��ġ(Position) ������ ����
	String::SplitString(&sString, scale.asString(), ","); // ũ��(Scale) ������ ����
	String::SplitString(&rString, rotation.asString(), ","); // ȸ��(Rotation) ������ ����

	// 10. ���ڿ� �����͸� float ������ ��ȯ�Ͽ� ����(Vector)�� ����
	Vector3 p = Vector3(stof(pString[0]), stof(pString[1]), stof(pString[2])); // ��ġ
	Vector3 s = Vector3(stof(sString[0]), stof(sString[1]), stof(sString[2])); // ũ��
	Vector3 r = Vector3(stof(rString[0]), stof(rString[1]), stof(rString[2])); // ȸ��

	// 11. ���� ��ġ, ũ��, ȸ���� ����
	GetRelativeTransform()->SetPosition(p);
	GetRelativeTransform()->SetScale(s);
	GetRelativeTransform()->SetRotation(r.x, r.y, r.z);

	stream.close();

	// 12. ��Ƽ����� �޽� �����͸� �ε�
	ReadMaterial(String::ToWString(material.asString())); // ��Ƽ���� �ε�
	ReadMesh(String::ToWString(mesh.asString())); // �޽� �ε�


}


/**
 * @param InFilePath ".material" ������ ���� ��� \n
 * ���͸��� ������ �о� MaterialTable�� ����
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