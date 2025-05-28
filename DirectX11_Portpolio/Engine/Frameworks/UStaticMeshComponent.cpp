#include "HeaderCollection.h"
#include "UStaticMeshComponent.h"
#include "StaticMesh.h"
#include "Utility/Converter.h"


/**
 * @param InFileName StaticMesh�� ���̳ʸ� ���� �̸�
 */
UStaticMeshComponent::UStaticMeshComponent(wstring InFileName, bool bOverwrite)
{
	wstring objectName = InFileName;
	InFileName = L"../Contents/_Objects/" + objectName + L".model";

	shared_ptr<Converter> converter = make_shared<Converter>();
	if (bOverwrite)
	{
		converter->ReadFile(objectName);
	}
	converter->ReadMeshInfo(InFileName, this);
}


/**
*  ��� ���͸��� ��ȯ
*/
vector<Material*> UStaticMeshComponent::GetAllMaterials()
{
	vector<Material*> result;

	for (auto& [name, matPtr] : MaterialTable)
	{
		if (matPtr)
			result.push_back(matPtr.get());
	}

	return result;
}

void UStaticMeshComponent::ReverseIndices()
{
	for (auto& mesh : m_Mesh)
	{
		for (UINT i = 0; i < mesh->Data.IndexCount; i += 3)
		{
			std::swap(mesh->Data.Indices[i + 1], mesh->Data.Indices[i + 2]);
		}
		mesh->CreateBuffer();
	}

}


void UStaticMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);
	
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->SetWorld(GetWorldTransform());
		meshPtr->Tick();
	}
	
}

/**
 * InitRenderer �Ϸ�Ǿ����� ȣ�� ����
 */
void UStaticMeshComponent::RenderComponent()
{
	Super::RenderComponent();

	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		meshPtr->Render();
	}
	
}

void UStaticMeshComponent::RenderMirror(const Matrix& refl)
{
	USceneComponent::RenderComponent();
	for (const shared_ptr<StaticMesh>& meshPtr : m_Mesh)
	{
		/*
		meshPtr->RenderMirror(refl);

		Renderer* renderer = meshPtr->GetMaterialData()->GetRenderer();

		// �ſ� ��ü�� ������ "Blend"�� �׸� �� ����
		//renderer->SetMirrorPipeline();
		FSceneView::Get()->PreRender();
		//renderer->SetDefaultRasterizeState();
		USceneComponent::RenderComponent();
		meshPtr->Render();
		D3D::Get()->ClearBlendState();
		//renderer->SetDefaultDepthStencilState();
		*/
	}
}

