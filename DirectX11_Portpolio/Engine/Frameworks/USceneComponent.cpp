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

	// �θ� Ʈ������ ��������
	if (AttachParent == nullptr)
	{
		// ��Ʈ: ���� Ʈ������ �״�� ���
		WorldTransform->SetTransform(*RelativeTransform.get());
		WorldBufferData.World = WorldTransform->ToMatrix().Transpose();
	}
	else
	{
		// �ڽ�: �θ� ���� Ʈ�������� �ռ�
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
 * @param InParent ���ο� AttachParent 
 * @param InSocketName ���� SoketName
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
 * @param InChild ���� �߰��� Child
 */
void USceneComponent::AddChild(USceneComponent* InChild)
{
	// nullptr üũ
	assert(InChild != nullptr && "Child pointer is null");

	// �̹� ��ϵǾ� �ִ��� �˻�
//	auto it = std::find(AttachChildren.begin(), AttachChildren.end(), InChild);
//	assert(it == AttachChildren.end() && "�̹� �ڽ����� ��� �� ������Ʈ�Դϴ�");

	// �ڽ� �߰�
	AttachChildren.push_back(InChild);
}


/**
 * @param InChild AttachChildren���� ���� �� Child
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
		assert(false && "��ϵǾ����� ���� �ڽ� ������Ʈ ����");
	}
}
