#pragma once
#include "EActorComponent.h"
#include "MathLibrary/FTransform.h"



class ESceneComponent : public EActorComponent
{
public:

	ESceneComponent();
	~ESceneComponent();

	FTransform Transform;

};