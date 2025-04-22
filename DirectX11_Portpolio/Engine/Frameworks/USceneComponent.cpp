#include "HeaderCollection.h"
#include "USceneComponent.h"

USceneComponent::USceneComponent()
{
	Transform = make_shared<FTransform>();
	
	WorldConstantBuffer = make_shared<ConstantBuffer>(&WorldBufferData, sizeof(WorldBufferData));
	WorldBufferData.World = Matrix::Identity;
}

USceneComponent::~USceneComponent()
{
}

void USceneComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	WorldBufferData.World = Transform->ToMatrix().Transpose();

}

void USceneComponent::RenderComponent()
{

	WorldConstantBuffer->UpdateConstBuffer();
	WorldConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::World, 1);
}

/*
 * 기존 부모에서 자신을 제거 후 새로운 부모의 child에 자신을등록 이후 기존 부모를 InParent로 설정
 */
void USceneComponent::SetUpAttachment(USceneComponent* InParent, const string& InSocketName)
{
	CheckNull(InParent);
	if (InParent != AttachParent || InSocketName != AttachSocketName)
	{
		AttachParent->RemoveChild(this);
		InParent->AddChild(this);
	}

	AttachParent = InParent;
	AttachSocketName = InSocketName;
}

void USceneComponent::AddChild(USceneComponent* Child)
{
	// nullptr 체크
	assert(Child != nullptr && "Child pointer is null");

	// 이미 등록되어 있는지 검사
	auto it = std::find(AttachChildren.begin(), AttachChildren.end(), Child);
	assert(it == AttachChildren.end() && "이미 자식으로 등록 된 컴포넌트입니다");

	// 자식 추가
	AttachChildren.push_back(Child);
}

void USceneComponent::RemoveChild(USceneComponent* Child)
{
	assert(Child != nullptr && "Child pointer is null");

	auto it = std::find(AttachChildren.begin(), AttachChildren.end(), Child);
	if (it != AttachChildren.end())
	{
		AttachChildren.erase(it);
	}
	else
	{
		assert(false && "등록되어있지 않은 자식 컴포넌트 제거");
	}
}
