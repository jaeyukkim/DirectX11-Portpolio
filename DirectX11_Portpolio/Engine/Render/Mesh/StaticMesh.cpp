#include "HeaderCollection.h"
#include "StaticMesh.h"



void StaticMesh::Tick()
{
}


void StaticMesh::SetWorld(const FTransform* InTransform)
{
	MeshWorld->SetPosition(InTransform->GetPosition());
	MeshWorld->SetRotation(InTransform->GetRotation());
	MeshWorld->SetScale(InTransform->GetScale());
}


void StaticMesh::CreateBuffer()
{
	VBuffer = make_shared<VertexBuffer>(Data.Vertices, Data.VertexCount, sizeof(VertexObject));
	IBuffer = make_shared<IndexBuffer>(Data.Indices, Data.IndexCount);
	MeshWorld = make_shared<FTransform>();
}