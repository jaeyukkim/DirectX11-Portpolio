#include "HeaderCollection.h"
#include "UMeshComponent.h"


UMeshComponent::~UMeshComponent()
{
}

void UMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

}

void UMeshComponent::RenderComponent()
{
	Super::RenderComponent();
}
