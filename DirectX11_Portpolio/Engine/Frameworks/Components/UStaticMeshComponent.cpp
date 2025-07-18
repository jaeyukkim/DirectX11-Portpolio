#include "HeaderCollection.h"
#include "UStaticMeshComponent.h"
#include "Render/Mesh/StaticMesh.h"
#include "Render/FSceneRender.h"




/**
 * @param InFileName StaticMesh의 바이너리 파일 이름
 * @param bOverwrite fbx 파일을 읽고 쓸 때 파일을 덮어쓸지
 */


UStaticMeshComponent::UStaticMeshComponent(wstring InMeshName, StaticMeshCreateInfo info)
	:MeshName(String::ToString(InMeshName))
{
	
	InMeshName = L"../../Contents/_Objects/" + InMeshName + L".model";

	shared_ptr<Converter> converter = make_shared<Converter>();
	converter->ReadBinary_ModelFile(InMeshName, this,
		FSceneRender::Get()->StaticMeshHasCreated(MeshName));

	
	if(!info.bIsMirror && !info.bIsSkyBox)
		FSceneRender::Get()->CreateMeshRenderProxy<StaticMeshRenderProxy>(MeshName, this);
}

UStaticMeshComponent::~UStaticMeshComponent()
{
}


/**
*  모든 메터리얼 반환
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
	if(InstanceID != nullptr)
	{
		TransformChanged.Broadcast(*InstanceID, WorldBufferData.World);
	}

}
