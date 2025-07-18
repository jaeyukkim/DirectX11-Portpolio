#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "ConsoleWindow.h"


ConsoleWindow::ConsoleWindow()
{
	SetName("Console");
	SetSize(ImVec2(200, 600));
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::Initialize()
{
}

void ConsoleWindow::Update()
{
	for (Editor* editor : mEditors)
	{
		editor->Update();
	}
}

void ConsoleWindow::OnGUI()
{

	for (Editor* editor : mEditors)
	{
		editor->OnGUI();
	}
}

void ConsoleWindow::Run()
{
	bool Active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &Active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void ConsoleWindow::OnEnable()
{
}

void ConsoleWindow::OnDisable()
{
}

void ConsoleWindow::OnDestroy()
{
}
