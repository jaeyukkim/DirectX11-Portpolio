
#include "HeaderCollection.h"
#include "IExecutable.h"
#include "Render/FSceneView.h"
#include "Render/PostProcess.h"
#include "Application.h"

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
	FSceneView::Create();

	Main = InMain;
	Main->Initialize();
	bIsRunning = true;
}



void Application::Destroy()
{
	Main->Destroy();
	Mouse::Destroy();
	Keyboard::Destroy();
	Timer::Destroy();
	FSceneView::Destroy();
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

	MainRender();
}

void Application::Close()
{
	bIsRunning = false;
}

bool Application::IsRunning()
{
	return bIsRunning;
}


void Application::MainRender()
{
	D3D::Get()->ClearFloatRTV();
	D3D::Get()->ClearRTV();
	D3D::Get()->ClearDSV();
	D3D::Get()->ClearBlendState();
	D3D::Get()->SetFloatRTV();
	FSceneView::Get()->PreRender();
	Main->Render();
	D3D::Get()->RunPostProcess();
	D3D::Get()->EndDraw();

	
}

