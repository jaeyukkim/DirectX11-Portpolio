#include "HeaderCollection.h"
#include "USkeletalMeshComponent.h"
#include "Render/Mesh/SkeletalMesh.h"
#include "Render/FSceneRender.h"

/**
 * @param InFileName SkeletalMesh의 바이너리 파일 이름
 * @param bOverwrite FBX 또는 gltf 파일을 읽어 다시 import할지 여부
 */
USkeletalMeshComponent::USkeletalMeshComponent(wstring InMeshName)
	:MeshName(String::ToString(InMeshName))
{
	wstring objectName = InMeshName;
	InMeshName = L"../../Contents/_Models/" + objectName + L".model";

	
	shared_ptr<Converter> converter = make_shared<Converter>();
	converter->ReadBinary_ModelFile(InMeshName, this,
		FSceneRender::Get()->SkeletalMeshHasCreated(MeshName));
	
	FSceneRender::Get()->CreateMeshRenderProxy<SkeletalMeshRenderProxy>(MeshName, this);
}

USkeletalMeshComponent::~USkeletalMeshComponent()
{
}


void USkeletalMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	for (const shared_ptr<SkeletalMesh>& meshPtr : m_Mesh)
	{
		meshPtr->SetWorld(GetWorldTransform());
		meshPtr->Tick();
	}

	TransformChanged.Broadcast(*InstanceID, WorldBufferData.World);

	AnimInstance->NativeUpdateAnimation(deltaTime);
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
