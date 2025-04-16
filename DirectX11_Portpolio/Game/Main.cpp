#include "Pch.h"
#include "Main.h"
#include "Systems/Application.h"
#include "TestGui.h"

void Main::Initialize()
{
	Push_Main(TestGui);
}

void Main::Destroy()
{
	for (IExecutable* execute : executes)
	{
		execute->Destroy();

		Delete(execute);
	}
}

void Main::Tick(float deltaTime)
{
	for (IExecutable* execute : executes)
		execute->Tick(deltaTime);
}

void Main::Render()
{
	for (IExecutable* execute : executes)
		execute->Render();
}

void Main::Push(IExecutable* execute)
{
	executes.push_back(execute);

	execute->Initialize();
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
		shared_ptr<Main> main = std::make_shared<Main>();
		wParam = Application::Run(main.get());
	}
	
	return wParam;
}
