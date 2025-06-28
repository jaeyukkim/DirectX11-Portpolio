#pragma once

#include "Editor/EditorWindow/EditorWindow.h"


enum class EDetailEditorType
{
	TransformEditor = 0
};
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


	unordered_map<EDetailEditorType, unique_ptr<Editor>> mEditors;
};