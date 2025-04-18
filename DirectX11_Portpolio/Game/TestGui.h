#pragma once


#include "Systems/IExecutable.h"

class Actor;

class TestGui : public IExecutable
{
public:
	void Initialize();
	void Destroy();

	void Tick(float deltaTime);
	void Render();

	void Interact();

	vector<shared_ptr<Actor>> Actors;

	float testval = 0.0f;
};