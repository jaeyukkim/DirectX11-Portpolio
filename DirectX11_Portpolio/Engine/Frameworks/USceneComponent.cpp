#include "HeaderCollection.h"
#include "USceneComponent.h"

USceneComponent::USceneComponent()
{
	Transform = make_shared<FTransform>();
	WorldConstantBuffer = make_shared<ConstantBuffer>(&WorldBufferData, sizeof(WorldBufferData));
}

USceneComponent::~USceneComponent()
{
}

void USceneComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	WorldBufferData.World = Transform->ToMatrix();
	
}

void USceneComponent::RenderComponent()
{

	WorldConstantBuffer->UpdateConstBuffer();
	WorldConstantBuffer->VSSetConstantBuffer(EConstBufferSlot::World, 1);
}
