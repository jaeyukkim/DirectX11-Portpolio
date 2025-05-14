#pragma once

#include "Editor/EditorWindow/EditorWindow.h"

class Editor;

class DetailWindow : public EditorWindow
{
public:
	DetailWindow();
	~DetailWindow();

	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void Run() override;
	void OnEnable() override;
	void OnDisable() override;
	void OnDestroy() override;

private:
	vector<unique_ptr<Editor>> mEditors;
};