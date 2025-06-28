#include "HeaderCollection.h"

#include "Editor/EditorWindow/EditorWindow.h"
#include "Editor/WindowView/ConsoleWindow.h"
#include "Editor/WindowView/ContentWindow.h"
#include "Editor/WindowView/DetailWindow.h"
#include "Editor/WindowView/OutlinerWindow.h"
#include "Frameworks/Objects/Actor.h"
#include <Systems/Application.h>

#include "EditorApplication.h"
#include <Editor/Widget/TransformWidget.h>




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
		if (auto detailWindow = dynamic_cast<DetailWindow*>(mEditorWindows[EEditorWindowType::DetailWindow].get()))
		{
			TransformWidget* transformWidget =  static_cast<TransformWidget*>(detailWindow->mEditors[EDetailEditorType::TransformEditor].get());

			FViewContext* viewContext = FSceneView::Get()->GetSceneViewContext();
			const Matrix* viewMatrix = &viewContext->View.Transpose();
			const Matrix* projectionMatrix = &viewContext->Projection.Transpose();

			Vector3 S = transformWidget->Scale;
			Vector3 R = transformWidget->Rotation;
			Vector3 T = transformWidget->Position;
			
			Matrix MS = Matrix::CreateScale(S);
			Matrix MR = Matrix::CreateFromYawPitchRoll(
				XMConvertToRadians(R.y),  // Yaw
				XMConvertToRadians(R.x),  // Pitch
				XMConvertToRadians(R.z)   // Roll
			);
			Matrix MT = Matrix::CreateTranslation(T);
			Matrix worldMatrix = MS * MR * MT;
			
			

			// snapping
			bool snap = Keyboard::Get()->Press(VK_LCONTROL);
			float snapValue = TranslateSnapVal;

			// snap to 45 degrees for rotation
			if (mGuizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = RotateSnapVal;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(*viewMatrix->m, *projectionMatrix->m, static_cast<ImGuizmo::OPERATION>(mGuizmoType)
				, ImGuizmo::WORLD, *worldMatrix.m, nullptr, snap ? snapValues : nullptr);

			bool isUsingGuizmo = ImGuizmo::IsUsing();
			if (isUsingGuizmo)
			{
				// Decompose matrix to translation, rotation and scale
				float translation[3];
				float rotation[3];
				float scale[3];
			
				ImGuizmo::DecomposeMatrixToComponents(*worldMatrix.m, translation, rotation, scale);

				// rotation은 degree 단위의 XYZ(Euler)
				transformWidget->Scale = Vector3(scale);
				transformWidget->Rotation = Vector3(rotation); // 바로 대입
				transformWidget->Position = Vector3(translation);
			}
			if(!isUsingGuizmo && mWasGuizmoUsingLastFrame)
			{
				transformWidget->UpdateTransform();
			}

			mWasGuizmoUsingLastFrame = isUsingGuizmo;

		}
		
	}
	
	ImGui::End();	// Scene end

	ImGui::PopStyleVar();
	ImGui::End(); // dockspace end
	
}