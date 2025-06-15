
#include "HeaderCollection.h"
#include "IExecutable.h"
#include "Application.h"

#include "Render/FSceneRender.h"

IExecutable* Application::Main = nullptr;
bool Application::bIsRunning = false;

Application::~Application()
{
}

Application::Application()
{
}

void Application::InitApplication(IExecutable* InMain)
{
	srand((UINT)time(0));

	D3D::Create();
	Timer::Create();
	Keyboard::Create();
	Mouse::Create();
	FSceneRender::Create();
	FSceneView::Create();
	
	Main = InMain;
	Main->Initialize();
	bIsRunning = true;
}



void Application::Destroy()
{
	
	Main->Destroy();
	FSceneView::Destroy();
	FSceneRender::Destroy();
	Mouse::Destroy();
	Keyboard::Destroy();
	Timer::Destroy();
	D3D::Destroy();

	D3DDesc desc = D3D::GetDesc();

	DestroyWindow(desc.Handle);
	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

void Application::Run()
{
	Timer::Get()->Tick();
	TimerManager::Get().Tick();
	float deltaTime = Timer::Get()->GetDeltaTime();
	Mouse::Get()->Tick(deltaTime);
	Main->Tick(deltaTime);

	//메인 렌더 루프
	FSceneRender::Get()->Render();
}

void Application::Close()
{
	bIsRunning = false;
}

bool Application::IsRunning()
{
	return bIsRunning;
}



