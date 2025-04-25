#pragma once
#include "Frameworks/UObject.h"

class Actor;

class UActorComponent : public UObject
{
public:
	UActorComponent();
	virtual ~UActorComponent();

public:
	virtual void TickComponent(float deltaTime);
	inline void SetOwner(Actor* InOwner) { OwnerPrivate = InOwner; }

protected:
	
	

private:
	Actor* OwnerPrivate;
	
};