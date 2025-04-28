#include "HeaderCollection.h"
#include "UPrimitiveComponent.h"


UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);
}

void UPrimitiveComponent::RenderComponent(bool bUsePreRender)
{
	Super::RenderComponent(bUsePreRender);
}
