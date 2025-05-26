#include "HeaderCollection.h"
#include "USceneComponent.h"

USceneComponent::USceneComponent()
{
	WorldTransform = make_shared<FTransform>();
	RelativeTransform = make_shared<FTransform>();
	
	WorldConstantBuffer = make_shared<ConstantBuffer>(&WorldBufferData, sizeof(WorldBufferData));
	WorldBufferData.World = Matrix::Identity;
}


void USceneComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	// 부모 트랜스폼 가져오기
	if (AttachParent == nullptr)
	{
		// 루트: 로컬 트랜스폼 그대로 사용
		WorldTransform->SetTransform(*RelativeTransform.get());
		WorldBufferData.World = WorldTransform->ToMatrix().Transpose();
	}
	else
	{
		// 자식: 부모 월드 트랜스폼과 합성
		Matrix WorldMat = RelativeTransform->ToMatrix() * AttachParent->WorldTransform->ToMatrix();
		WorldTransform->SetTransformFromMatrix(WorldMat);
		WorldBufferData.World = WorldTransform->ToMatrix().Transpose();
	}

}


void USceneComponent::RenderComponent()
{
	WorldConstantBuffer->UpdateConstBuffer();
	WorldConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::World, 1);
}


/**
 * @param InParent 새로운 AttachParent 
 * @param InSocketName 붙을 SoketName
 */
void USceneComponent::SetUpAttachment(USceneComponent* InParent, const string& InSocketName)
{
	CheckNull(InParent);
	
	if(AttachParent != nullptr)
		AttachParent->RemoveChild(this);
	
	InParent->AddChild(this);
	AttachParent = InParent;
	AttachSocketName = InSocketName;
	FTickTaskManager::bNeedUpdate = true;
}


void USceneComponent::Deteach()
{
	CheckNull(AttachParent);
	
	AttachParent->RemoveChild(this);
	AttachParent = nullptr;
	FTickTaskManager::bNeedUpdate = true;
}


/**
 * @param InChild 새로 추가할 Child
 */
void USceneComponent::AddChild(USceneComponent* InChild)
{
	// nullptr 체크
	assert(InChild != nullptr && "Child pointer is null");

	// 이미 등록되어 있는지 검사
//	auto it = std::find(AttachChildren.begin(), AttachChildren.end(), InChild);
//	assert(it == AttachChildren.end() && "이미 자식으로 등록 된 컴포넌트입니다");

	// 자식 추가
	AttachChildren.push_back(InChild);
}


/**
 * @param InChild AttachChildren에서 제거 할 Child
 */
void USceneComponent::RemoveChild(USceneComponent* InChild)
{
	assert(InChild != nullptr && "Child pointer is null");

	auto it = std::find(AttachChildren.begin(), AttachChildren.end(), InChild);
	if (it != AttachChildren.end())
	{
		AttachChildren.erase(it);
	}
	else
	{
		assert(false && "등록되어있지 않은 자식 컴포넌트 제거");
	}
}
