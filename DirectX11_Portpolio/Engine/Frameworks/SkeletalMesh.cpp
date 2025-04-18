#include "HeaderCollection.h"
#include "Utility/BinaryFile.h"
#include "SkeletalMesh.h"



SkeletalMesh::SkeletalMesh()
{
	
}

SkeletalMesh::~SkeletalMesh()
{

}

void SkeletalMesh::Tick()
{
	//FrameRender->Tick();

	BoneData.BoneIndex = BoneIndex;
	memcpy(BoneData.Transforms, Transforms, sizeof(Matrix) * MAX_MODEL_TRANSFORM);

	//World->Tick();
	// scale, transform, rotation 업데이트
}



void SkeletalMesh::Render()
{

	VBuffer->IASetVertexBuffer();
	IBuffer->IASetIndexBuffer();

	BoneBuffer->UpdateConstBuffer();
	BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::Bone, 1);

	MaterialData->Render();
	
	
	//World->RenderComponent();


	MaterialData->GetRenderer()->DrawIndexed(IBuffer->GetCount());
		
	
}

void SkeletalMesh::SetWorld(const FTransform* InTransform)
{
	MeshWorld->SetPosition(InTransform->GetPosition());
	MeshWorld->SetRotationFromEuler(InTransform->GetRotation());
	MeshWorld->SetScale(InTransform->GetScale());
}



void SkeletalMesh::ReadFile(BinaryReader* InReader, const map<string, shared_ptr<Material>>& InMaterialTable, vector<shared_ptr<SkeletalMesh>>& OutMeshes)
{
    // 1. 메시 개수 읽기
    UINT count = InReader->FromUInt();

    // 2. 메시 개수만큼 반복하여 데이터를 읽음
    for (UINT i = 0; i < count; i++)
    {
        // 2.1 새로운 메시 객체 생성
		auto mesh = make_shared<SkeletalMesh>();

        // 2.2 메시 이름 읽기
        mesh->Name = InReader->FromString();

        // 2.3 메시가 사용할 머티리얼(Material) 이름 읽기
        string materialName = InReader->FromString();

        // 2.4 머티리얼 테이블에서 해당 머티리얼 찾기
        mesh->MaterialData = InMaterialTable.at(materialName).get();

        // 2.5 정점(Vertex) 개수 읽기
        mesh->VertexCount = InReader->FromUInt();

        // 2.6 정점 데이터 할당 및 바이너리 파일에서 읽기
    	
    	mesh->Vertices = new VertexModel[mesh->VertexCount];
    	InReader->FromByte(mesh->Vertices, sizeof(VertexModel) * mesh->VertexCount);

    	// 2.7 인덱스(Index) 개수 읽기
    	mesh->IndexCount = InReader->FromUInt();

    	// 2.8 인덱스 데이터 할당 및 바이너리 파일에서 읽기
    	mesh->Indices = new UINT[mesh->IndexCount];
    	InReader->FromByte(mesh->Indices, sizeof(UINT) * mesh->IndexCount);

        // 2.9 메시 리스트(OutMeshes)에 메시 추가
        OutMeshes.push_back(mesh);
    }

    // 3. 모든 메시에 대해 GPU 버퍼 생성
	for (shared_ptr<SkeletalMesh>& mesh : OutMeshes)
		mesh->CreateBuffer();
}

void SkeletalMesh::CreateBuffer()
{
	VBuffer = make_shared<VertexBuffer>(Vertices, VertexCount, sizeof(VertexModel));
	IBuffer = make_shared<IndexBuffer>(Indices, IndexCount);
		
	
	BoneBuffer = make_shared<ConstantBuffer>(&BoneData, sizeof(BoneDesc));
	
	MeshWorld = make_shared<FTransform>();
}
