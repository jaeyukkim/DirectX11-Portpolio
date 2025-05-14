#include "Pch.h"
#include "Systems/Application.h"

ATOM MyRegisterClass(HINSTANCE hInInstance);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);
void OpenConsoleWindow();

int WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE InPrevInstance,
	_In_ LPSTR InParam, _In_ int command)
{

#ifdef _DEBUG

	OpenConsoleWindow();

#endif

	MyRegisterClass(hInInstance);

	shared_ptr<World> main = make_shared<World>();
	Application::InitApplication(main.get());
	EditorApplication::InitApplication();
	

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (Application::IsRunning()) //Game Loop
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
			Application::Run();
			EditorApplication::Run();
			D3D::Get()->Present();
		}
	}
	EditorApplication::Destroy();
	Application::Destroy();

	return msg.wParam;
	
}

ATOM MyRegisterClass(HINSTANCE hInInstance)
{
	ATOM check;

	D3DDesc desc;
	desc.AppName = L"KimJaeYuk";
	desc.Instance = hInInstance;
	desc.Handle = nullptr;
	desc.Width = 1600;
	desc.Height = 900;

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

		check = RegisterClassEx(&wndClass);
		assert(check != 0);
	}
	{
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
	}


	D3D::SetDesc(desc);

	return check;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	if (ImGui_ImplWin32_WndProcHandler(InHandle, InMessage, InwParam, InlParam))
		return TRUE;


	if (InMessage == WM_SIZE)
	{
		float width = (float)LOWORD(InlParam);
		float height = (float)HIWORD(InlParam);

		if (D3D::Get() != nullptr)
			D3D::Get()->ResizeScreen(width, height);
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

void OpenConsoleWindow()
{

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		ios::sync_with_stdio(false);
	}

	cout << "Console Open" << endl;
}