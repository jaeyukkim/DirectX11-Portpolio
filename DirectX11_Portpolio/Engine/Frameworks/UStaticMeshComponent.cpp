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
UStaticMeshComponent::UStaticMeshComponent(wstring InFileName, bool bOverwirte)
{

	wstring objectName = InFileName;
	InFileName = L"../Contents/_Objects/" + objectName + L".model";


	if (bOverwirte)
	{
		shared_ptr<Converter> converter = make_shared<Converter>();
		converter->ReadFile(objectName + L"/" + objectName + L".fbx");
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
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 8 bytes
		{ "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
		{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
		{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
	};

	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->MaterialData->GetRenderer()->InitRenderer(inputElements, meshPtr->MaterialData->GetSamplerDesc());
	}

	bInitRenderComplete = true;
}

/**
* @param InMaterialName : ã�� ���͸��� Ű ����
*/
Material* UStaticMeshComponent::GetMaterial(string InMaterialName)
{
	auto it = MaterialTable.find(InMaterialName);
	if (it != MaterialTable.end())
	{
		return it->second.get();
	}

	return nullptr;
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

	/*
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->SetWorld(GetTransform());
		meshPtr->Tick();
	}
	*/
	m_Mesh[0]->SetWorld(GetWorldTransform());
	m_Mesh[0]->Tick();
}

/**
 * InitRenderer �Ϸ�Ǿ����� ȣ�� ����
 */
void UStaticMeshComponent::RenderComponent(bool bUsePreRender)
{
	Super::RenderComponent(bUsePreRender);

	/*for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->Render();
	}*/
	m_Mesh[0]->Render(bUsePreRender);
}

void UStaticMeshComponent::DrawComponentIndex()
{
	m_Mesh[0]->DrawIndexed();
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

	// 1. ��Ƽ���� ����(.material)�� ��θ� ����
	InFilePath = L"../Contents/_Objects/" + InFilePath + L".material";

	// 2. �ش� ������ ��ġ�� ���� ��� ��������
	wstring textureFolder = Path::GetDirectoryName(InFilePath);

	// 3. JSON ������ �б� ���� �Է� ��Ʈ�� ����
	ifstream stream;

	// 4. ������ ����
	stream.open(InFilePath);

	// 5. JSON �����͸� ������ ��ü ����
	Json::Value root;

	// 6. JSON ���Ͽ��� �����͸� �о� `root` ��ü�� ����
	stream >> root;

	// 7. JSON���� ��� ��Ƽ����(Key ��) ��� ��������
	Json::Value::Members members = root.getMemberNames();

	// 8. �� ��Ƽ���� ������ ��ȸ�ϸ鼭 �о����
	for (string name : members)
	{
		// 8.1 ���ο� ��Ƽ���� ��ü ����
		shared_ptr<Material> material = make_shared<Material>();

		// 8.2 ���� ��Ƽ������ ������ ��������
		Json::Value value = root[name];

		// 8.3 ���̴�(Shader) �̸��� �����ϴ� ���, ���̴� ����
		if (value["VertexShaderPath"].asString().size() > 0)
			material->GetRenderer()->SetVertexShaderPath(String::ToWString(value["VertexShaderPath"].asString()));
		if (value["PixelShaderPath"].asString().size() > 0)
			material->GetRenderer()->SetPixelShaderPath(String::ToWString(value["PixelShaderPath"].asString()));


		// 8.4 ��Ƽ���� ���� ���� ���� (Ambient, Diffuse, Specular, Emissive)
		material->SetAmbient(JsonStringToColor(value["Ambient"].asString()));
		material->SetDiffuse(JsonStringToColor(value["Diffuse"].asString()));
		material->SetSpecular(JsonStringToColor(value["Specular"].asString()));
		material->SetEmissive(JsonStringToColor(value["Emissive"].asString()));

		// 8.5 �ؽ�ó(DiffuseMap) ����
		if (value["DiffuseMap"].asString().size() > 0)
			material->SetDiffuseMap(String::ToWString(value["DiffuseMap"].asString()));

		// 8.6 �ؽ�ó(SpecularMap) ����
		if (value["SpecularMap"].asString().size() > 0)
			material->SetSpecularMap(String::ToWString(value["SpecularMap"].asString()));

		// 8.7 �ؽ�ó(NormalMap) ����
		if (value["NormalMap"].asString().size() > 0)
			material->SetNormalMap(String::ToWString(value["NormalMap"].asString()));

		// 8.8 ���� ��Ƽ������ MaterialTable�� ���� (name�� Key�� ���)
		MaterialTable[name] = material;
	}

	// 9. ���� ��Ʈ���� ����
	stream.close();
}


void UStaticMeshComponent::ReadMesh(wstring InFilePath)
{
	// 1. ���� ��θ� ���� (�� �����Ͱ� ����� ���)
	InFilePath = L"../Contents/_Objects/" + InFilePath + L".mesh";


	// 2. ���� ������ �б� ���� BinaryReader ��ü ����
	unique_ptr<BinaryReader> reader = make_unique<BinaryReader>();

	// 3. ������ .mesh ������ ����
	reader->Open(InFilePath);


	// 5. �޽�(Mesh) �����͸� �о `Meshes` ���� �� ��Ƽ����(MaterialTable)�� ����
	StaticMesh::ReadFile(reader.get(), MaterialTable, m_Mesh);

	// 6. ���� �ݱ� �� �޸� ����
	reader->Close();


}


Color UStaticMeshComponent::JsonStringToColor(string InString)
{
	vector<string> v;
	String::SplitString(&v, InString, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}