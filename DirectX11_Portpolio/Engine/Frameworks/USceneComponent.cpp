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
 * ���� �θ𿡼� �ڽ��� ���� �� ���ο� �θ��� child�� �ڽ������ ���� ���� �θ� InParent�� ����
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
	// nullptr üũ
	assert(Child != nullptr && "Child pointer is null");

	// �̹� ��ϵǾ� �ִ��� �˻�
	auto it = std::find(AttachChildren.begin(), AttachChildren.end(), Child);
	assert(it == AttachChildren.end() && "�̹� �ڽ����� ��� �� ������Ʈ�Դϴ�");

	// �ڽ� �߰�
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
		assert(false && "��ϵǾ����� ���� �ڽ� ������Ʈ ����");
	}
}
