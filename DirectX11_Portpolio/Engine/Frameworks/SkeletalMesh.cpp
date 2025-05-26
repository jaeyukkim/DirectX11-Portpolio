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

    for (UINT i = 0; i < count; i++)
    {
		auto mesh = make_shared<SkeletalMesh>();

        mesh->Name = InReader->FromString();
        string materialName = InReader->FromString();
        mesh->MaterialData = InMaterialTable.at(materialName).get();

        mesh->BoneData.BoneIndex = InReader->FromUInt();

        mesh->VertexCount = InReader->FromUInt();
    	mesh->ModelVertices = new VertexModel[mesh->VertexCount];
    	InReader->FromByte(mesh->ModelVertices, sizeof(VertexModel) * mesh->VertexCount);

    	mesh->IndexCount = InReader->FromUInt();
    	mesh->Indices = new UINT[mesh->IndexCount];
    	InReader->FromByte(mesh->Indices, sizeof(UINT) * mesh->IndexCount);

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
