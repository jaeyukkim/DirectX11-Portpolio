#pragma once

#include "Systems/IExecutable.h"


struct FHitResult;
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
	

public:
	static ULevel* GetLevel();
	static void SphereTraceMulti(vector<FHitResult>& hitResults, Vector3 location, float radius);
	static void PushLog(const string& msg, Color color = Color(1, 1, 1, 1));
	
private:
	static ULevel* PersistentLevel;
	
};
