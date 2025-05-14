#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "ContentWindow.h"


ContentWindow::ContentWindow()
{
	SetName("Content");
	SetSize(ImVec2(300, 600));
}

ContentWindow::~ContentWindow()
{
}

void ContentWindow::Initialize()
{
}

void ContentWindow::Update()
{
	for (Editor* editor : mEditors)
	{
		editor->Update();
	}
}

void ContentWindow::OnGUI()
{

	for (Editor* editor : mEditors)
	{
		editor->OnGUI();
	}
}

void ContentWindow::Run()
{
	bool Active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &Active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void ContentWindow::OnEnable()
{
}

void ContentWindow::OnDisable()
{
}

void ContentWindow::OnDestroy()
{
}
