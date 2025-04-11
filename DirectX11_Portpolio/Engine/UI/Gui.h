#pragma once
#pragma once

class Gui
{
public:
	

public:
	static void Create();
	static void Destroy();

	static Gui* Get();

public:
	void Tick(float deltaTime);
	void Render();
	
private:
	Gui();
	~Gui();
	


public:
	LRESULT WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

public:
	void RenderText(float x, float y, float r, float g, float b, string content);
	void RenderText(float x, float y, float r, float g, float b, float a, string content);
	
private:
	static Gui* Instance;

private:
	
	

private:
	struct GuiText
	{
		XMFLOAT2 Position;
		XMFLOAT4 Color;
		string Content;
	};

private:
	vector<GuiText> Contents;
};