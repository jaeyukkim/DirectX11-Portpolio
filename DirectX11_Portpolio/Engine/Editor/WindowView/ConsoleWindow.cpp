#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "ConsoleWindow.h"
vector<LogEntry> ConsoleWindow::LogBuffer;
char ConsoleWindow::SearchBuffer[256] = "";

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

	// Search Box
	ImGui::InputText("Search", SearchBuffer, IM_ARRAYSIZE(SearchBuffer));

	// Scrollable Log Output
	ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	for (const auto& log : LogBuffer)
	{
		if (strlen(SearchBuffer) == 0 || strstr(log.Message.c_str(), SearchBuffer))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, log.Color);
			ImGui::TextUnformatted(log.Message.c_str());
			ImGui::PopStyleColor();
		}
	}

	ImGui::EndChild();
	
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
