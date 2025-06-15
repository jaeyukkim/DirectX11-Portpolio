#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "DetailWindow.h"
#include "Editor/Widget/TransformWidget.h"
#include "Render/PostProcess/PostProcess.h"
#include "Render/PostProcess/ImageFilter.h"



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

	ImageFilter& CombineFilter = D3D::Get()->GetPostProcess()->CombineFilter;
	FLightInfo* LInfo = FSceneView::Get()->GetLightInfo();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Processing"))
	{

		int flag = 0;
		flag += ImGui::SliderFloat("Bloom Strength", &CombineFilter.FilterData.strength, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Exposure", &CombineFilter.FilterData.option1, 0.0f, 10.0f);
		flag += ImGui::SliderFloat("Gamma", &CombineFilter.FilterData.option2, 0.1f, 5.0f);
		flag += ImGui::SliderFloat("IBLStrength", &LInfo->IBLStrength, 0.0f, 5.0f);

		// 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
		if (flag)
		{
			CombineFilter.UpdateConstantBuffers();
		}

		ImGui::TreePop();
	}

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
