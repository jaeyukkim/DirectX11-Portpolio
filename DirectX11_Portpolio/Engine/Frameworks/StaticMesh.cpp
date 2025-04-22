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
	MaterialData->GetRenderer()->Bind();	// VS, PS 등 스테이지 바인딩
	BindRenderStage();   //파생 클래스 바인딩 먼저 진행( 본 정보 세팅 등)

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
	
	// 1. 메시 개수 읽기
	UINT count = InReader->FromUInt();

	// 2. 메시 개수만큼 반복하여 데이터를 읽음
	for (UINT i = 0; i < count; i++)
	{
		// 2.1 새로운 메시 객체 생성
		auto mesh = make_shared<StaticMesh>();

		// 2.2 메시 이름 읽기
		mesh->Name = InReader->FromString();

		// 2.3 메시가 사용할 머티리얼(Material) 이름 읽기
		string materialName = InReader->FromString();

		// 2.4 머티리얼 테이블에서 해당 머티리얼 찾기
		mesh->MaterialData = InMaterialTable.at(materialName).get();

		// 2.5 정점(Vertex) 개수 읽기
		mesh->VertexCount = InReader->FromUInt();

		// 2.6 정점 데이터 할당 및 바이너리 파일에서 읽기

		mesh->Vertices = new VertexObject[mesh->VertexCount];
		InReader->FromByte(mesh->Vertices, sizeof(VertexObject) * mesh->VertexCount);

		// 2.7 인덱스(Index) 개수 읽기
		mesh->IndexCount = InReader->FromUInt();

		// 2.8 인덱스 데이터 할당 및 바이너리 파일에서 읽기
		mesh->Indices = new UINT[mesh->IndexCount];
		InReader->FromByte(mesh->Indices, sizeof(UINT) * mesh->IndexCount);

		// 2.9 메시 리스트(OutMeshes)에 메시 추가
		OutMeshes.push_back(mesh);
	}

	// 3. 모든 메시에 대해 GPU 버퍼 생성
	for (shared_ptr<StaticMesh>& mesh : OutMeshes)
		mesh->CreateBuffer();
	
}


void StaticMesh::CreateBuffer()
{
	VBuffer = make_shared<VertexBuffer>(Vertices, VertexCount, sizeof(VertexObject));
	IBuffer = make_shared<IndexBuffer>(Indices, IndexCount);
	MeshWorld = make_shared<FTransform>();
}


