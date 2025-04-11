#pragma once


#include "Systems/IExecutable.h"



class TestGui : public IExecutable
{
public:
	void Initialize();
	void Destroy();

	void Tick(float deltaTime);
	void Render();

	void Interact();


	float testval = 0.0f;
};