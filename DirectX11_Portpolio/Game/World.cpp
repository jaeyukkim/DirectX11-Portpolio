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

void World::Render()
{
	PersistentLevel->Render();
}

ULevel* World::GetLevel()
{
	return PersistentLevel;
}


int WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE InPrevInstance, 
                   _In_ LPSTR InParam,_In_ int command)
{
	D3DDesc desc;
	desc.AppName = L"KimJaeYuk";
	desc.Instance = hInInstance;
	desc.Handle = nullptr;

	desc.Width = 1600;
	desc.Height = 900;
	

	D3D::SetDesc(desc);

	
	WPARAM wParam;
	{
		shared_ptr<World> main = std::make_shared<World>();
		wParam = Application::Run(main.get());
	}
	
	return wParam;
}
