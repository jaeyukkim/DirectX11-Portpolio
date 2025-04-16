#pragma once
#include "EObject.h"
#include "ESceneComponent.h"


class Actor : public EObject
{
public:
	Actor();
	~Actor();

	
	shared_ptr<ESceneComponent> RootComponent;
};
