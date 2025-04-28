#pragma once

#include "Systems/IExecutable.h"


class ULevel;

class World : public IExecutable
{
public:
	World() = default;
	~World() = default;
	
public:
	void Initialize();
	void Destroy();

	void Tick(float deltaTime);
	void Render();

	static ULevel* GetLevel();
	
private:
	static ULevel* PersistentLevel;
	
};
