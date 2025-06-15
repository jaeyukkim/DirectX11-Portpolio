#include "HeaderCollection.h"
#include "Mouse.h"

Mouse* Mouse::Instance = nullptr;

Mouse::Mouse() :
	Position(Vector3(0.0f, 0.0f, 0.0f)), WheelOldStatus(Vector3(0.0f, 0.0f, 0.0f)), WheelMoveDelta(Vector3(0.0f, 0.0f, 0.0f)), WheelStatus(Vector3(0.0f, 0.0f, 0.0f))
{
	ZeroMemory(ButtonStatus, sizeof(BYTE) * (int)MouseButton::Max);
	ZeroMemory(ButtonOldStatus, sizeof(BYTE) * (int)MouseButton::Max);
	ZeroMemory(ButtonMaps, sizeof(MouseButtonState) * (int)MouseButton::Max);

	D3D::Get()->WinSizeChanged.Add([this]()
		{
			UpdateScreenCenter();
		});

	UpdateScreenCenter();
}

Mouse::~Mouse()
{
}


void Mouse::Create()
{
	assert(Instance == nullptr);

	Instance = new Mouse();
}

void Mouse::Destroy()
{
	assert(Instance != nullptr);
	Delete(Instance);
}

Mouse* Mouse::Get()
{
	return Instance;
}

bool Mouse::Down(int InType)
{
	return Down((MouseButton)InType);
}

bool Mouse::Up(int InType)
{
	return Up((MouseButton)InType);
}

bool Mouse::Press(int InType)
{
	return Press((MouseButton)InType);
}

bool Mouse::Down(MouseButton InType)
{
	return ButtonMaps[(int)InType] == MouseButtonState::Down;
}

bool Mouse::Up(MouseButton InType)
{
	return ButtonMaps[(int)InType] == MouseButtonState::Up;
}

bool Mouse::Press(MouseButton InType)
{
	return ButtonMaps[(int)InType] == MouseButtonState::Press;
}

void Mouse::Tick(float deltaTime)
{
	memcpy(ButtonOldStatus, ButtonStatus, sizeof(BYTE) * (int)MouseButton::Max);

	ZeroMemory(ButtonStatus, sizeof(BYTE) * (int)MouseButton::Max);
	ZeroMemory(ButtonMaps, sizeof(MouseButtonState) * (int)MouseButton::Max);

	ButtonStatus[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	ButtonStatus[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	ButtonStatus[2] = GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;

	for (int i = 0; i < (int)MouseButton::Max; i++)
	{
		int status = ButtonStatus[i];
		int oldStatus = ButtonOldStatus[i];

		if (oldStatus == 0 && status == 1)
			ButtonMaps[i] = MouseButtonState::Down;
		else if (oldStatus == 1 && status == 0)
			ButtonMaps[i] = MouseButtonState::Up;
		else if (oldStatus == 1 && status == 1)
			ButtonMaps[i] = MouseButtonState::Press;
		else
			ButtonMaps[i] = MouseButtonState::None;
	}

	
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(D3D::GetDesc().Handle, &point);

	WheelOldStatus.x = WheelStatus.x;
	WheelOldStatus.y = WheelStatus.y;

	WheelStatus.x = (float)point.x;
	WheelStatus.y = (float)point.y;

	WheelMoveDelta = WheelStatus - WheelOldStatus;
	WheelOldStatus.z = WheelStatus.z;

	/*
	* //  �ΰ��ӿ� ���콺 ��ǥ����
		POINT screenPoint;
		GetCursorPos(&screenPoint); // ��ũ�� ��ǥ�迡�� ���콺 ��ġ ��������

		WheelMoveDelta = Vector3(
			(float)(screenPoint.x - ScreenCenter.x),
			(float)(screenPoint.y - ScreenCenter.y),
			0.0f);
	

		// 3. �ٽ� ���콺 Ŀ���� ����� ��ġ��Ŵ (��ũ�� ��ǥ ����)
		SetCursorPos(ScreenCenter.x, ScreenCenter.y);
	*/
	
}

void Mouse::UpdateScreenCenter()
{
	D3DDesc desc = D3D::GetDesc();

	// Ŭ���̾�Ʈ �߽��� ���� ���
	POINT center = { desc.Width / 2, desc.Height / 2 };

	// Ŭ���̾�Ʈ �߽��� ��ũ�� ��ǥ��� ��ȯ
	ClientToScreen(desc.Handle, &center);

	// ��ȯ�� ��ũ�� ��ǥ ����
	ScreenCenter.x = center.x;
	ScreenCenter.y = center.y;
}

void Mouse::WndProc(UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	if (InMessage == WM_MOUSEMOVE)
	{
		Position.x = (float)LOWORD(InlParam);
		Position.y = (float)HIWORD(InlParam);
	}

	if (InMessage == WM_MOUSEWHEEL)
	{
		WheelOldStatus.z = WheelStatus.z;
		WheelStatus.z += (float)((short)HIWORD(InwParam));
	}
}


