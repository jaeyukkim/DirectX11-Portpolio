#include "HeaderCollection.h"
#include "Keyboard.h"

Keyboard* Keyboard::Instance = nullptr;

void Keyboard::Create()
{
	assert(Instance == nullptr);

	Instance = new Keyboard();
}

void Keyboard::Destroy()
{
	assert(Instance != nullptr);

	Delete(Instance);
}

Keyboard* Keyboard::Get()
{
	return Instance;
}

Keyboard::Keyboard()
{
	for (int i = 0; i < MAX_KEY_COUNT; i++)
	{
		UpList.set(i, false);
		DownList.set(i, false);
	}
}

Keyboard::~Keyboard()
{

}

bool Keyboard::Down(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		if (DownList[InKey] == false)
		{
			DownList.set(InKey, true);

			return true;
		}
	}
	else
	{
		DownList.set(InKey, false);
	}

	return false;
}

bool Keyboard::Up(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		DownList.set(InKey, true);
	}
	else
	{
		if (DownList[InKey] == true)
		{
			DownList.set(InKey, false);

			return true;
		}
	}

	return false;
}

bool Keyboard::Press(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
		return true;

	return false;
}

bool Keyboard::Toggle(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x0001)
		return true;

	return false;
}
