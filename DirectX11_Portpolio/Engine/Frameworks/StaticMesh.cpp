#include "HeaderCollection.h"
#include "StaticMesh.h"
#include "UStaticMeshComponent.h"
#include "Utility/Converter.h"
#include <Render/FSceneView.h>




void StaticMesh::Tick()
{
}

void StaticMesh::Render()
{
	VBuffer->IASetVertexBuffer();
	IBuffer->IASetIndexBuffer();
	Data.MaterialData->BindMaterial();
	Renderer::Get()->DrawIndexed(IBuffer->GetCount());
}

void StaticMesh::RenderMirror(const Matrix& refl)
{
	/*
	Renderer* renderer = MaterialData->GetRenderer();

	
	// �ſ� ��ġ�� StencilBuffer�� 1�� ǥ��
	renderer->SetStencilMaskPipeline();
	VBuffer->IASetVertexBuffer();
	IBuffer->IASetIndexBuffer();
	renderer->DrawIndexed(IBuffer->GetCount());
	
	// �ſ� ��ġ�� �ݻ�� ��ü���� ������
	renderer->SetReflactPipeline();
	FSceneView::Get()->UpdateReflactRow(refl);
	D3D::Get()->ClearOnlyDepth();
	World::GetLevel()->Render();
	*/
	
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