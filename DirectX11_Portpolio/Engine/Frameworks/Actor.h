#pragma once
#include "UObject.h"
#include "USceneComponent.h"


class Actor : public UObject
{
public:
	Actor();
	virtual ~Actor();
	virtual void Tick(float deltaTime);
	virtual void Render();
	
	shared_ptr<USceneComponent> RootComponent;
};
