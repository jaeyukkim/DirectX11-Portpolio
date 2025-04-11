#include "HeaderCollection.h"
#include "Gui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);


Gui* Gui::Instance = nullptr;

void Gui::Create()
{
	assert(Instance == nullptr);

	Instance = new Gui();

}

void Gui::Destroy()
{
	assert(Instance != nullptr);

	Delete(Instance);
}

Gui* Gui::Get()
{
	return Instance;
}

void Gui::Tick(float deltaTime)
{
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	
}

void Gui::Render()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGui::Begin
	(
		"TextWindow", NULL,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus
	);
	
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


}

LRESULT Gui::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	if (ImGui::GetCurrentContext() == nullptr)
		return 0;

	return ImGui_ImplWin32_WndProcHandler(InHandle, InMessage, InwParam, InlParam);

}

void Gui::RenderText(float x, float y, float r, float g, float b, string content)
{
	RenderText(x, y, r, g, b, 1.0f, content);
}

void Gui::RenderText(float x, float y, float r, float g, float b, float a, string content)
{
	GuiText text;
	text.Position = XMFLOAT2(x, y);
	text.Color = XMFLOAT4(r, g, b, a);
	text.Content = content;

	Contents.push_back(text);
}

Gui::Gui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	const D3DDesc& desc = D3D::GetDesc();

	assert(ImGui_ImplWin32_Init(desc.Handle), "Not Init Imgui");
	assert(ImGui_ImplDX11_Init(D3D::Get()->GetDevice(), D3D::Get()->GetDeviceContext()), "Not Init Imgui");
}

Gui::~Gui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}
