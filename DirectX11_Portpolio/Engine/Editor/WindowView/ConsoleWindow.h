#pragma once

#include "Editor/EditorWindow/EditorWindow.h"

class Editor;

class ConsoleWindow : public EditorWindow
{
public:
	ConsoleWindow();
	~ConsoleWindow();

	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void Run() override;
	void OnEnable() override;
	void OnDisable() override;
	void OnDestroy() override;

	static void AddLog(const string& msg, ImVec4 color = ImVec4(1, 1, 1, 1))
	{
		LogBuffer.push_back({ msg, color });
	}
private:
	vector<Editor*> mEditors;

	static vector<LogEntry> LogBuffer;
	static char SearchBuffer[256];
};