#include "HeaderCollection.h"
#include "SkeletalMesh.h"


void SkeletalMesh::Tick()
{
	BoneIdxData.BoneIndex = Data.BoneIndex;
	memcpy(BoneData.Transforms, Data.Transforms, sizeof(Matrix) * MAX_MODEL_TRANSFORM);
	memcpy(BoneData.OffsetTransforms, Data.OffsetTransforms, sizeof(Matrix) * MAX_MODEL_TRANSFORM);
}

void SkeletalMesh::Render()
{
	BoneBuffer->UpdateConstBuffer();
	BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::CB_Bone, 1);
}

void SkeletalMesh::SetWorld(const FTransform* InTransform)
{
	MeshWorld->SetPosition(InTransform->GetPosition());
	MeshWorld->SetRotation(InTransform->GetRotation());
	MeshWorld->SetScale(InTransform->GetScale());
}

void SkeletalMesh::CreateBuffer()
{
	VBuffer = make_shared<VertexBuffer>(Data.ModelVertices, Data.VertexCount, sizeof(VertexModel));
	IBuffer = make_shared<IndexBuffer>(Data.Indices, Data.IndexCount);
	MeshWorld = make_shared<FTransform>();
	BoneBuffer = make_shared<ConstantBuffer>(&BoneData, sizeof(BoneDesc));
	BoneIndexCBuffer = make_shared<ConstantBuffer>(&BoneIdxData, sizeof(BoneIDX));
}