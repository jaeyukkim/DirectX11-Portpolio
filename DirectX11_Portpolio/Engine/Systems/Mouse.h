#pragma once

enum class MouseButton
{
	Left = 0, Right, Middle, Max,
};

class Mouse
{
public:
	static void Create();
	static void Destroy();

	static Mouse* Get();

public:
	Vector3 GetPosition() { return Position; }
	Vector3 GetMoveDelta() { return WheelMoveDelta; }

public:
	bool Down(int InType);
	bool Up(int InType);
	bool Press(int InType);

	bool Down(MouseButton InType);
	bool Up(MouseButton InType);
	bool Press(MouseButton InType);

public:
	void Tick(float deltaTime);

	void WndProc(UINT InMessage, WPARAM InwParam, LPARAM InlParam);

private:
	static Mouse* Instance;

private:
	Mouse();
	~Mouse();

private:
	enum class MouseButtonState
	{
		None = 0, Down, Up, Press, DoubleClick, Max,
	};

private:
	Vector3 Position;

	BYTE ButtonStatus[(int)MouseButton::Max];
	BYTE ButtonOldStatus[(int)MouseButton::Max];
	MouseButtonState ButtonMaps[(int)MouseButton::Max];

	Vector3 WheelStatus;
	Vector3 WheelOldStatus;
	Vector3 WheelMoveDelta;
};
