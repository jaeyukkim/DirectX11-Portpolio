#include "HeaderCollection.h"

#include "Editor/EditorWindow/EditorWindow.h"
#include "Editor/WindowView/ConsoleWindow.h"
#include "Editor/WindowView/ContentWindow.h"
#include "Editor/WindowView/DetailWindow.h"
#include "Editor/WindowView/OutlinerWindow.h"
#include "Frameworks/Actor.h"
#include <Systems/Application.h>

#include "EditorApplication.h"
#include <Render/FSceneView.h>



EditorApplication* EditorApplication::Instance = nullptr;


void EditorApplication::SetSelectedActor(weak_ptr<Actor> actor)
{
	mSelectedActor = actor;

	// weak_ptr → shared_ptr → raw pointer
	if (auto shared = actor.lock())
	{
		Actor* rawPtr = shared.get();
		SeletedActorChanged.Broadcast(rawPtr);
	}
	else
	{
		// 선택 해제된 경우 nullptr 브로드캐스트
		SeletedActorChanged.Broadcast(nullptr);
	}
}


Actor* EditorApplication::GetSelectedActor()
{
	if (shared_ptr<Actor> actor = mSelectedActor.lock())
	{
		return actor.get();
	}
	return nullptr;
}

EditorApplication::EditorApplication()
	:mFlag(ImGuiWindowFlags_None), mState(EditorApplication::eState::Active), mDockspaceFlags(ImGuiDockNodeFlags_None),
	mFullScreen(true), mViewportFocused(false), mViewportHovered(false), mGuizmoType(ImGuizmo::OPERATION::TRANSLATE)
{

}

bool EditorApplication::InitApplication()
{
	assert(Instance == nullptr);
	Instance = new EditorApplication();
	

	Gui::Create();

	//CosoleWindow
	shared_ptr<ConsoleWindow> console = make_shared<ConsoleWindow>();
	Instance->mEditorWindows.insert(make_pair(EEditorWindowType::ConsoleWindow, console));
	
	//ContentWindow
	shared_ptr<ContentWindow> content = make_shared<ContentWindow>();
	Instance->mEditorWindows.insert(make_pair(EEditorWindowType::ContentWindow, content));

	//DetailWindow
	shared_ptr<DetailWindow> detail = make_shared<DetailWindow>();
	Instance->mEditorWindows.insert(make_pair(EEditorWindowType::DetailWindow, detail));

	//HierarchyWindow
	shared_ptr<OutlinerWindow> outliner = make_shared<OutlinerWindow>();
	Instance->mEditorWindows.insert(make_pair(EEditorWindowType::OutlinerWindow, outliner));

	return true;
}


void EditorApplication::Destroy()
{
	assert(Instance != nullptr);
	Gui::Destroy();


#ifdef _DEBUG
	FreeConsole();
#endif
	Delete(Instance);
}


void EditorApplication::Update()
{

}

void EditorApplication::OnGUI()
{
	Gui::Get()->Tick();
	Instance->OnImGuiRender();
	Gui::Get()->Render();
}


void EditorApplication::Run()
{
	Instance->Update();
	Instance->OnGUI();
}


EditorApplication* EditorApplication::Get()
{
	return Instance;
}

void EditorApplication::OpenProject()
{

}

void EditorApplication::NewScene()
{

}

void EditorApplication::SaveScene()
{

}

void EditorApplication::SaveSceneAs()
{

}

void EditorApplication::OpenScene(const std::filesystem::path& path)
{


}

void EditorApplication::OnImGuiRender()
{
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	mFlag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (mFullScreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		mFlag |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		mFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (mDockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		mFlag |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	bool Active = static_cast<bool>(mState);
	ImGui::Begin("EditorApplication", &Active, mFlag);
	ImGui::PopStyleVar();

	if (mFullScreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 200.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), mDockspaceFlags);
	}

	style.WindowMinSize.x = minWinSizeX;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
				OpenProject();

			ImGui::Separator();

			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				NewScene();

			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				SaveSceneAs();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				Application::Close();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Script"))
		{
			if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
			{
				//ScriptEngine::ReloadAssembly(); 추후 C#스크립트 추가기능이 생기면 추가할 예정
			}

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}

	for (auto& iter : mEditorWindows)
		iter.second->Run();

	// viewport
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene");

	const auto viewportMinRegion = ImGui::GetWindowContentRegionMin(); // 씬뷰의 최소 좌표
	const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax(); // 씬뷰의 최대 좌표
	const auto viewportOffset = ImGui::GetWindowPos(); // 씬뷰의 위치

	constexpr int letTop = 0;
	constexpr int rightBottom = 1;
	mViewportBounds[letTop] = Vector2{ viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	mViewportBounds[rightBottom] = Vector2{ viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	// check if the mouse,keyboard is on the Sceneview
	mViewportFocused = ImGui::IsWindowFocused();
	mViewportHovered = ImGui::IsWindowHovered();

	// to do : mouse, keyboard event

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	mViewportSize = Vector2{ viewportPanelSize.x, viewportPanelSize.y };
	ImGui::Image((ImTextureID)D3D::Get()->GetRenderTargetSRV(), ImVec2{mViewportSize.x, mViewportSize.y}
	, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });

	// Open Scene by drag and drop
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ITEM"))
		{
			const auto path = static_cast<const wchar_t*>(payload->Data);
			OpenScene(path);
		}
		ImGui::EndDragDropTarget();
	}
	
	// To do : guizmo
	Actor* selectedObject = GetSelectedActor();
	if (selectedObject && mGuizmoType != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetGizmoSizeClipSpace(0.15f);
		ImGuizmo::SetRect(mViewportBounds[0].x, mViewportBounds[0].y
			, mViewportBounds[1].x - mViewportBounds[0].x, mViewportBounds[1].y - mViewportBounds[0].y);

		// To do : guizmo...
		// game view camera setting

		// Scene Camera
		FViewContext* viewContext = FSceneView::Get()->GetSceneViewContext();
		const Matrix* viewMatrix = &viewContext->View.Transpose();
		const Matrix* projectionMatrix = &viewContext->Projection.Transpose();

		// Object Transform
		FTransform* transform = selectedObject->GetActorTransform();
		Matrix worldMatrix = transform->ToMatrix();

		// snapping
		bool snap = Keyboard::Get()->Press(VK_LCONTROL);
		float snapValue = TranslateSnapVal;

		// snap to 45 degrees for rotation
		if (mGuizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = RotateSnapVal;

		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(*viewMatrix->m, *projectionMatrix->m, static_cast<ImGuizmo::OPERATION>(mGuizmoType)
			, ImGuizmo::WORLD, *worldMatrix.m, nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			// Decompose matrix to translation, rotation and scale
			float translation[3];
			float rotation[3];
			float scale[3];
			ImGuizmo::DecomposeMatrixToComponents(*worldMatrix.m, translation, rotation, scale);

			// delta rotation from the current rotation
			Vector3 deltaRotation = Vector3(rotation) - transform->GetRotation();
			deltaRotation = transform->GetRotation() + deltaRotation;

			// set the new transform
			transform->SetScale(Vector3(scale));
			transform->SetRotation(Vector3(deltaRotation));
			transform->SetPosition(Vector3(translation));
		}
	}
	
	ImGui::End();	// Scene end

	ImGui::PopStyleVar();
	ImGui::End(); // dockspace end
	
}