#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "OutlinerWindow.h"
#include "Frameworks/Objects/Actor.h"


OutlinerWindow::OutlinerWindow()
{
	SetName("Outliner");
	SetSize(ImVec2(200, 600));
}

OutlinerWindow::~OutlinerWindow()
{
}

void OutlinerWindow::Initialize()
{
}

void OutlinerWindow::Update()
{
	for (Editor* editor : mEditors)
	{
		editor->Update();
	}
}

void OutlinerWindow::OnGUI()
{

	for (Editor* editor : mEditors)
	{
		editor->OnGUI();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4)); // 패딩 최소화
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // 버튼 간 간격 제거

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));      // 배경색 (회색, 반투명)
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.7f)); // 호버 색
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.9f));  // 클릭 시 색

	Actor* selectedActor = EditorApplication::Get()->GetSelectedActor();
	for (const auto& actor : World::GetLevel()->GetAllActor())
	{
	    if (!actor) continue;

		std::string name = actor->GetObjectName();
		bool isSelected = false;
		if(selectedActor != nullptr)
		{
			isSelected = selectedActor->GetObjectID() == actor->GetObjectID();
		}

		// full row size
		ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, 22.0f);

		if (ImGui::Selectable(name.c_str(), isSelected, 0, size))
		{
			selectedActor = actor.get();
			EditorApplication::Get()->SetSelectedActor(actor);
		}
	}

ImGui::PopStyleColor(3);
ImGui::PopStyleVar(2);
}

void OutlinerWindow::Run()
{
	bool Active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &Active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void OutlinerWindow::OnEnable()
{
}

void OutlinerWindow::OnDisable()
{
}

void OutlinerWindow::OnDestroy()
{
}
