#include "HeaderCollection.h"
#include "StaticMesh.h"

#include "Utility/BinaryFile.h"


StaticMesh::StaticMesh()
{

}

StaticMesh::~StaticMesh()
{

}

void StaticMesh::Tick()
{
	
}


void StaticMesh::BindRenderStage()
{

}

void StaticMesh::Render()
{
	MaterialData->GetRenderer()->Bind();	// VS, PS �� �������� ���ε�
	BindRenderStage();   //�Ļ� Ŭ���� ���ε� ���� ����( �� ���� ���� ��)

	VBuffer->IASetVertexBuffer();
	IBuffer->IASetIndexBuffer();
	MaterialData->Render();
	MaterialData->GetRenderer()->DrawIndexed(IBuffer->GetCount());
}

void StaticMesh::SetWorld(const FTransform* InTransform)
{
	MeshWorld->SetPosition(InTransform->GetPosition());
	MeshWorld->SetRotationFromEuler(InTransform->GetRotation());
	MeshWorld->SetScale(InTransform->GetScale());
}



void StaticMesh::ReadFile(BinaryReader* InReader, 
	const map<string, shared_ptr<Material>>& InMaterialTable, vector<shared_ptr<StaticMesh>>& OutMeshes)
{
	
	// 1. �޽� ���� �б�
	UINT count = InReader->FromUInt();

	// 2. �޽� ������ŭ �ݺ��Ͽ� �����͸� ����
	for (UINT i = 0; i < count; i++)
	{
		// 2.1 ���ο� �޽� ��ü ����
		auto mesh = make_shared<StaticMesh>();

		// 2.2 �޽� �̸� �б�
		mesh->Name = InReader->FromString();

		// 2.3 �޽ð� ����� ��Ƽ����(Material) �̸� �б�
		string materialName = InReader->FromString();

		// 2.4 ��Ƽ���� ���̺��� �ش� ��Ƽ���� ã��
		mesh->MaterialData = InMaterialTable.at(materialName).get();

		// 2.5 ����(Vertex) ���� �б�
		mesh->VertexCount = InReader->FromUInt();

		// 2.6 ���� ������ �Ҵ� �� ���̳ʸ� ���Ͽ��� �б�

		mesh->Vertices = new VertexObject[mesh->VertexCount];
		InReader->FromByte(mesh->Vertices, sizeof(VertexObject) * mesh->VertexCount);

		// 2.7 �ε���(Index) ���� �б�
		mesh->IndexCount = InReader->FromUInt();

		// 2.8 �ε��� ������ �Ҵ� �� ���̳ʸ� ���Ͽ��� �б�
		mesh->Indices = new UINT[mesh->IndexCount];
		InReader->FromByte(mesh->Indices, sizeof(UINT) * mesh->IndexCount);

		// 2.9 �޽� ����Ʈ(OutMeshes)�� �޽� �߰�
		OutMeshes.push_back(mesh);
	}

	// 3. ��� �޽ÿ� ���� GPU ���� ����
	for (shared_ptr<StaticMesh>& mesh : OutMeshes)
		mesh->CreateBuffer();
	
}


void StaticMesh::CreateBuffer()
{
	VBuffer = make_shared<VertexBuffer>(Vertices, VertexCount, sizeof(VertexObject));
	IBuffer = make_shared<IndexBuffer>(Indices, IndexCount);
	MeshWorld = make_shared<FTransform>();
}


