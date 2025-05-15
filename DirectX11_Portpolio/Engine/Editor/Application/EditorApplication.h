#pragma once
#include <filesystem>
#include "Utility/Delegate.h"



class EditorWindow;
class Actor;

DECLARE_DYNAMIC_DELEGATE_OneParam(FSeletedActorChangedDelegate, Actor*);

enum class EEditorWindowType
{
	ConsoleWindow = 1,
	ContentWindow = 2,
	DetailWindow = 3,
	OutlinerWindow = 4
};
class EditorApplication
{
public:
	enum class eState
	{
		Disable,
		Active,
		Destroy,
	};

	template <typename T>
	T* GetWindow(const EEditorWindowType& name)
	{
		auto iter = mEditorWindows.find(name);
		if (iter == mEditorWindows.end())
			return nullptr;

		return dynamic_cast<T*>(iter->second);
	}

	///<summary>
	///에디터를 초기화합니다.
	///</summary>
	static bool InitApplication();

	/// <summary>
	/// 에디터를 로직을 업데이트합니다.
	/// </summary>
	void Update();

	/// <summary>
	/// 에디터 렌더링을 업데이트합니다.
	/// </summary>
	void OnGUI();

	/// <summary>
	/// 에디터 업데이트
	/// </summary>
	static void Run();

	/// <summary>
	/// 에디터를 종료합니다.
	/// </summary>
	static void Destroy();

	static EditorApplication* Get();

	void OpenProject();
	void NewScene();
	void SaveScene();
	void SaveSceneAs();
	void OpenScene(const std::filesystem::path& path);
	void OnImGuiRender();

	//Event
	void SetKeyPressed(int keyCode, int scancode, int action, int mods);
	void SetCursorPos(double x, double y);
	void SetGuizmoType(int type) { mGuizmoType = type; }

public:
	void SetSelectedActor(weak_ptr<Actor> actor);
	Actor* GetSelectedActor();
	FSeletedActorChangedDelegate SeletedActorChanged;

private:
	EditorApplication();
	~EditorApplication() = default;

private:

	static EditorApplication* Instance;
	map<EEditorWindowType, shared_ptr<EditorWindow>> mEditorWindows;
	weak_ptr<Actor> mSelectedActor;


private:
	ImGuiWindowFlags mFlag;
	ImGuiDockNodeFlags mDockspaceFlags;
	eState mState;
	bool mFullScreen;
	Vector2 mViewportBounds[2];
	Vector2 mViewportSize;
	bool mViewportFocused;
	bool mViewportHovered;
	int mGuizmoType;

private:
	const float TranslateSnapVal = 0.5f;
	const float RotateSnapVal = 10.0f;


	
};