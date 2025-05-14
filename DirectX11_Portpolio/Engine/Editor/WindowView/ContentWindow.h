#pragma once

#include "Editor/EditorWindow/EditorWindow.h"

class Editor;

class ContentWindow : public EditorWindow
{
public:
	ContentWindow();
	~ContentWindow();

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