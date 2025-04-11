#pragma once

#include "HeaderCollection.h"
#include "IExecutable.h"
#include "Application.h"

IExecutable* Application::Main = nullptr;


Application::~Application()
{
}

Application::Application()
{
}


WPARAM Application::Run(IExecutable* InMain)
{
	srand((UINT)time(0));

	Create();

	 D3D::Create();
	 Timer::Create();
	 Gui::Create();
	 Keyboard::Create();
	 Mouse::Create();

	Main = InMain;
	Main->Initialize();
	

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true) //Game Loop
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg); //WM_CHAR
			DispatchMessage(&msg);
		}
		else
		{
		
			MainRender();
		}
	}


	Destroy();

	return msg.wParam;
}

void Application::Create()
{
	D3DDesc desc = D3D::GetDesc();

	//Regist Window Class
	{
		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = desc.Instance;
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = desc.AppName.c_str();

		ATOM check = RegisterClassEx(&wndClass);
		assert(check != 0);
	}

	//Create Window Handle
	desc.Handle = CreateWindowEx
	(
		0,
		desc.AppName.c_str(),
		desc.AppName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, //Default - X
		CW_USEDEFAULT, //Default - Y
		(int)desc.Width,
		(int)desc.Height,
		nullptr,
		nullptr,
		desc.Instance,
		nullptr
	);
	 
	assert((desc.Handle != nullptr));

	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow(desc.Handle, centerX, centerY, rect.right - rect.left, rect.bottom - rect.top, TRUE);

	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);
	ShowCursor(true);

	D3D::SetDesc(desc);
}

void Application::Destroy()
{
	Main->Destroy();
	Mouse::Destroy();
	Keyboard::Destroy();
	Timer::Destroy();
	Gui::Destroy();
	D3D::Destroy();

	D3DDesc desc = D3D::GetDesc();

	DestroyWindow(desc.Handle);
	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

LRESULT Application::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	if (Gui::Get()->WndProc(InHandle, InMessage, InwParam, InlParam))
		return TRUE;


	if (InMessage == WM_SIZE)
	{
		if (Main != nullptr)
		{
			float width = (float)LOWORD(InlParam);
			float height = (float)HIWORD(InlParam);

			if (D3D::Get() != nullptr)
				D3D::Get()->ResizeScreen(width, height);
		}
	}

	if (InMessage == WM_KEYDOWN)
	{
		
		if (InwParam == VK_ESCAPE)
		{
			PostQuitMessage(0);

			return 0;
		}

	}

	if (InMessage == WM_CLOSE || InMessage == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(InHandle, InMessage, InwParam, InlParam);
}

void Application::MainRender()
{
	Timer::Get()->Tick();
	float deltaTime = Timer::Get()->GetDeltaTime();
	Gui::Get()->Tick(deltaTime);
	Mouse::Get()->Tick(deltaTime);
	Main->Tick(deltaTime);

	//Rendering
	{
	
		D3D::Get()->ClearRenderTargetView();

		Main->Render();

		Gui::Get()->Render();

		D3D::Get()->Present();
	}
}

