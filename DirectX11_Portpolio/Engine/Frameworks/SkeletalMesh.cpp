#include "HeaderCollection.h"
#include "Utility/Converter.h"
#include "SkeletalMesh.h"


void SkeletalMesh::Tick()
{
	BoneData.BoneIndex = Data.BoneIndex;
	memcpy(BoneData.Transforms, Data.Transforms, sizeof(Matrix) * MAX_MODEL_TRANSFORM);
}

void SkeletalMesh::Render()
{
	BoneBuffer->UpdateConstBuffer();
	BoneBuffer->VSSetConstantBuffer(EConstBufferSlot::Bone, 1);
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
}
