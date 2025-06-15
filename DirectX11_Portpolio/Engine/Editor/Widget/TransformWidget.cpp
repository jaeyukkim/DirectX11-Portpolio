#include "HeaderCollection.h"
#include "TransformWidget.h"
#include "Frameworks/Objects/Actor.h"


TransformWidget::TransformWidget(Actor* target)
    :BoxSize(ImVec2(0, 110)), BoxColor(ImVec4(0.2f, 0.2f, 0.2f, 0.3f))
{
    if (target)
        mTransform = target->GetActorTransform();
    else
        mTransform = nullptr;
}

void TransformWidget::OnGUI()
{
    if (!mTransform)
        return;

    auto pos = mTransform->GetPosition();
    auto rot = mTransform->GetRotation();
    auto scale = mTransform->GetScale();

    float posArr[3] = { pos.x, pos.y, pos.z };
    float rotArr[3] = { rot.x, rot.y, rot.z };
    float scaleArr[3] = { scale.x, scale.y, scale.z };

    // 스타일 저장
    ImGui::PushStyleColor(ImGuiCol_ChildBg, BoxColor); // 회색 반투명 배경
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));

    // 외곽 박스 시작
    if (ImGui::BeginChild("TransformBox", ImVec2(0, 120), true, ImGuiWindowFlags_None))
    {
        ImGui::Text("Transform");

        ImGui::Separator();

        if (ImGui::DragFloat3("Position", posArr, 0.1f))
            mTransform->SetPosition({ posArr[0], posArr[1], posArr[2] });

        if (ImGui::DragFloat3("Rotation", rotArr, 1.0f))
            mTransform->SetRotation({ rotArr[0], rotArr[1], rotArr[2] });

        if (ImGui::DragFloat3("Scale", scaleArr, 0.1f))
            mTransform->SetScale({ scaleArr[0], scaleArr[1], scaleArr[2] });
    }

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}