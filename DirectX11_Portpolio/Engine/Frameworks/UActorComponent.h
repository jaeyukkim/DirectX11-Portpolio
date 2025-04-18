#pragma once
#include "Frameworks/UObject.h"

class UActorComponent : public UObject
{
public:
	UActorComponent();
	virtual ~UActorComponent();

	virtual void TickComponent(float deltaTime);
};