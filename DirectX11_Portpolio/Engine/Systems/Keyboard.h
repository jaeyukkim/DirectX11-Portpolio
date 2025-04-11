#pragma once

#define MAX_KEY_COUNT 256

class Keyboard
{
public:
	static void Create();
	static void Destroy();

	static Keyboard* Get();

private:
	static Keyboard* Instance;

public:
	bool Down(int InKey);
	bool Up(int InKey);

	bool Press(int InKey);
	bool Toggle(int InKey);

private:
	Keyboard();
	~Keyboard();

private:
	bitset<MAX_KEY_COUNT> UpList;
	bitset<MAX_KEY_COUNT> DownList;
};