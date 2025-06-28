#include "HeaderCollection.h"
#include "USkeletalMeshComponent.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/FSceneRender.h"

/**
 * @param InFileName SkeletalMesh의 바이너리 파일 이름
 */
USkeletalMeshComponent::USkeletalMeshComponent(wstring InFileName, bool bOverwrite)
{
	wstring objectName = InFileName;
	InFileName = L"../../Contents/_Models/" + objectName + L".model";
	
	shared_ptr<Converter> converter = make_shared<Converter>();
	if (bOverwrite)
	{
		converter->ReadFile(objectName, EMeshType::SkeletalMeshType);
	}
	converter->ReadMeshInfo(InFileName, this);

	FSceneRender::Get()->CreateRenderProxy<SkeletalMeshRenderProxy>(this);
}


void USkeletalMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	for (const shared_ptr<SkeletalMesh>& meshPtr : m_Mesh)
	{
		meshPtr->SetWorld(GetWorldTransform());
		meshPtr->Tick();
	}
}


vector<Material*> USkeletalMeshComponent::GetAllMaterials()
{
	vector<Material*> result;

	for (auto& [name, matPtr] : MaterialTable)
	{
		if (matPtr)
			result.push_back(matPtr.get());
	}

	return result;
}
