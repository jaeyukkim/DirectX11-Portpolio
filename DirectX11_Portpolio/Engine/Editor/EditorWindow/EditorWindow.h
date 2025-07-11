#pragma once

#include "Editor/Entity/Entity.h"

class EditorWindow : public Entity
{
public:
	enum class EWindowState
	{
		Disable,
		Active,
		Destroy,
	};

	EditorWindow();
	virtual ~EditorWindow();

	virtual void Initialize();
	virtual void Update();
	virtual void OnGUI();
	virtual void Run();
	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();
	virtual void SelectedActorChange(Actor* actor);

	ImGuiWindowFlags GetFlag() const { return mFlag; }
	EWindowState GetState() const { return mState; }
	void SetState(EWindowState state) { mState = state; }
	ImVec2 GetSize() { return mSize; }
	void SetSize(ImVec2 size) { mSize = size; }

private:
	ImGuiWindowFlags mFlag;
	EWindowState mState;
	ImVec2 mSize;

};