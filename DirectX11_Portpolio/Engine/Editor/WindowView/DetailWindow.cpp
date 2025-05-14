#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "DetailWindow.h"
#include "Editor/Widget/TransformWidget.h"


DetailWindow::DetailWindow()
{
	SetName("Detail");
	SetSize(ImVec2(200, 600));
	EditorApplication::Get()->SeletedActorChanged.Add([this](Actor* actor)
		{
			// 1. 기존 위젯 제거 (unique_ptr이므로 clear만 하면 됨)
			mEditors.clear();

	// 2. 유효한 액터일 경우 Transform 위젯 추가
			if (actor)
			{
				mEditors.emplace_back(make_unique<TransformWidget>(actor));
			}
		});

}

DetailWindow::~DetailWindow()
{
}

void DetailWindow::Initialize()
{
}

void DetailWindow::Update()
{
	for (const auto& editor : mEditors)
	{
		editor->Update();
	}
}

void DetailWindow::OnGUI()
{

	for (const auto& editor : mEditors)
	{
		editor->OnGUI();
	}
}

void DetailWindow::Run()
{
	bool Active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &Active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void DetailWindow::OnEnable()
{
}

void DetailWindow::OnDisable()
{
}

void DetailWindow::OnDestroy()
{
}
