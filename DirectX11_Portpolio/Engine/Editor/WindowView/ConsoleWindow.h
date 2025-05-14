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

private:
	vector<Editor*> mEditors;
};