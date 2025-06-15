#include "Pch.h"
#include "World.h"
#include "ULevel.h"
#include "Systems/Application.h"

ULevel* World::PersistentLevel = nullptr;

void World::Initialize()
{
	PersistentLevel = new ULevel();
	PersistentLevel->Initialize();
}

void World::Destroy()
{
	Delete(PersistentLevel);
}

void World::Tick(float deltaTime)
{
	PersistentLevel->Tick(deltaTime);
}

ULevel* World::GetLevel()
{
	return PersistentLevel;
}