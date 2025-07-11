#include "HeaderCollection.h"
#include "TransformWidget.h"
#include "Frameworks/Objects/Actor.h"


TransformWidget::TransformWidget(Actor* target)
    :BoxSize(ImVec2(0, 110)), BoxColor(ImVec4(0.2f, 0.2f, 0.2f, 0.3f))
{
    
    ActorTransform = target->GetActorTransform();
    if (ActorTransform)
    {
        Position = ActorTransform->GetPosition();
        Rotation = ActorTransform->GetRotation();
        Scale = ActorTransform->GetScale();
    }
    
}

void TransformWidget::OnGUI()
{
    if (ActorTransform == nullptr)
        return;
    
    
    // 스타일 저장
    ImGui::PushStyleColor(ImGuiCol_ChildBg, BoxColor); // 회색 반투명 배경
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));

    // 외곽 박스 시작
    if (ImGui::BeginChild("TransformBox", ImVec2(0, 120), true, ImGuiWindowFlags_None))
    {
        ImGui::Text("Transform");

        ImGui::Separator();

        if (ImGui::DragFloat3("Position", &Position.x, 0.1f))
            ActorTransform->SetPosition({ Position.x, Position.y, Position.z });

        if (ImGui::DragFloat3("Rotation", &Rotation.x, 0.1f))
            ActorTransform->SetRotation({ Rotation.x, Rotation.y, Rotation.z });

        if (ImGui::DragFloat3("Scale", &Scale.x, 0.1f))
            ActorTransform->SetScale({ Scale.x, Scale.y, Scale.z });
    }

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
}

void TransformWidget::UpdateTransformWidget()
{
    Editor::Update();

    if (ActorTransform)
    {
        ActorTransform->SetPosition(Position);
        ActorTransform->SetRotation(Rotation);
        ActorTransform->SetScale(Scale);
    }
}
