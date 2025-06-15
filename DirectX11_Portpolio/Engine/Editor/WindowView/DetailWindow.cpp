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
			// 1. ���� ���� ���� (unique_ptr�̹Ƿ� clear�� �ϸ� ��)
			mEditors.clear();

	// 2. ��ȿ�� ������ ��� Transform ���� �߰�
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

		// ���ǻ� ����� �Է��� �νĵǸ� �ٷ� GPU ���۸� ������Ʈ
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
