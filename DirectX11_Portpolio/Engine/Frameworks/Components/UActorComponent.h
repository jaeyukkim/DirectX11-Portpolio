#pragma once
#include "Frameworks/Objects/UObject.h"

class Actor;

class UActorComponent : public UObject
{
public:
	UActorComponent();
	virtual ~UActorComponent();

public:
	virtual void TickComponent(float deltaTime);
	inline void SetOwner(Actor* InOwner) { OwnerPrivate = InOwner; }


private:
	Actor* OwnerPrivate;
	
};