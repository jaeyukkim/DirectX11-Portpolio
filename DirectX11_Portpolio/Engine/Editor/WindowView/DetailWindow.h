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

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnGUI() override;
	virtual void Run() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;
	virtual void SelectedActorChange(Actor* actor) override;

	unordered_map<EDetailEditorType, unique_ptr<Editor>> mEditors;
};