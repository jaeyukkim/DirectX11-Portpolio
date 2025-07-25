#include "HeaderCollection.h"
#include "UPrimitiveComponent.h"



void UPrimitiveComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	if(InstanceID != nullptr)
	{
		TransformChanged.Broadcast(*InstanceID, WorldBufferData.World);
	}
}
