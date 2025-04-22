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
	Super::Tick();

	BoneData.BoneIndex = BoneIndex;
	memcpy(BoneData.Transforms, Transforms, sizeof(Matrix) * MAX_MODEL_TRANSFORM);

	
}


void SkeletalMesh::BindRenderStage()    
{
	BoneBuffer->UpdateConstBuffer();
	BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::Bone, 1);

    Super::BindRenderStage(); //파생 클래스 정보 먼저 바인딩하고 다음 상위클래스 바인딩
}


void SkeletalMesh::Render()
{
	Super::Render();
	
}

void SkeletalMesh::SetWorld(const FTransform* InTransform)
{
    Super::SetWorld(InTransform);
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
    	
    	mesh->ModelVertices = new VertexModel[mesh->VertexCount];
    	InReader->FromByte(mesh->ModelVertices, sizeof(VertexModel) * mesh->VertexCount);

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
	VBuffer = make_shared<VertexBuffer>(ModelVertices, VertexCount, sizeof(VertexModel));
	IBuffer = make_shared<IndexBuffer>(Indices, IndexCount);
	MeshWorld = make_shared<FTransform>();
	BoneBuffer = make_shared<ConstantBuffer>(&BoneData, sizeof(BoneDesc));
}
