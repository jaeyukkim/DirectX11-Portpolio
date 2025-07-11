#include "HeaderCollection.h"
#include "Editor/Application/guiEditor.h"
#include "DetailWindow.h"
#include "Editor/Widget/TransformWidget.h"
#include "Render/PostProcess/PostProcess.h"
#include "Render/PostProcess/ImageFilter.h"
#include <Render/FSceneRender.h>
#include "Render/PostEffects/PostEffect.h"



DetailWindow::DetailWindow()
{
	SetName("Detail");
	SetSize(ImVec2(200, 600));
	EditorApplication::Get()->SeletedActorChanged.Add(this, &DetailWindow::SelectedActorChange);

}

DetailWindow::~DetailWindow()
{
}

void DetailWindow::Initialize()
{
}

void DetailWindow::Update()
{
	for (auto& pair : mEditors)
	{
		if (pair.second) 
			pair.second->Update();
	}
}

void DetailWindow::OnGUI()
{

	for (auto& pair : mEditors)
	{
		if (pair.second)
			pair.second->OnGUI();
	}
}

void DetailWindow::Run()
{
	bool Active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &Active, GetFlag());

	Update();
	OnGUI();

	ImageFilter& CombineFilter = D3D::Get()->GetPostProcess()->CombineFilter;
	PostEffectsData& postEffectData = FSceneRender::Get()->GetPostEffect()->GetPostEffectData();
	FLightObjects* light = FSceneView::Get()->GetLights();
	FLightInfo* lightInfo = FSceneView::Get()->GetLightInfo();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Processing"))
	{

		int flag = 0;
		flag += ImGui::SliderFloat("Bloom Strength", &CombineFilter.FilterData.strength, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("threshold", &CombineFilter.FilterData.threshold, 0.0f, 3.0f);
		flag += ImGui::SliderFloat("IBLStrength", &lightInfo->IBLStrength, 0.0f, 5.0f);
		flag += ImGui::SliderFloat("FogStrength", &postEffectData.FogStrength, 0.0f, 25.0f);

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

void DetailWindow::SelectedActorChange(Actor* actor)
{
	auto it = mEditors.find(EDetailEditorType::TransformEditor);
	if (it != mEditors.end()) 
	{
		mEditors.erase(it);
	}
		
	// 2. 유효한 액터일 경우 Transform 위젯 추가
	if (actor)
	{
		mEditors.insert({ EDetailEditorType::TransformEditor, make_unique<TransformWidget>(actor) });
	}
}
