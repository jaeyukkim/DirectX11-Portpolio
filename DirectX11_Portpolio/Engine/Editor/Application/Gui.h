#pragma once
#pragma once

class Gui
{
public:
	static void Create();
	static void Destroy();

	static Gui* Get();

public:
	void Tick();
	void Render();
	
private:
	Gui();
	~Gui();
	

public:
	void RenderText(float x, float y, float r, float g, float b, string content);
	void RenderText(float x, float y, float r, float g, float b, float a, string content);
	
private:
	void SetDarkThemeColors();

private:
	static Gui* Instance;



private:
	struct GuiText
	{
		Vector2 Position;
		Color Color;
		string Content;
	};

private:
	vector<GuiText> Contents;
};