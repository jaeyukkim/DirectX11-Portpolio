#pragma once

#include "Systems/IExecutable.h"



class Main : public IExecutable
{
public:
	void Initialize();
	void Destroy();

	void Tick(float deltaTime);
	void Render();

	

private:
	void Push(IExecutable* execute);

private:
	vector<IExecutable*> executes;
};