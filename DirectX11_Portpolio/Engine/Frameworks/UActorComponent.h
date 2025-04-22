#pragma once
#include "Frameworks/UObject.h"

class Actor;

class UActorComponent : public UObject
{
public:
	UActorComponent();
	virtual ~UActorComponent();

	virtual void TickComponent(float deltaTime);



private:
	weak_ptr<Actor> OwnerPrivate;
};