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
    
    
    // ��Ÿ�� ����
    ImGui::PushStyleColor(ImGuiCol_ChildBg, BoxColor); // ȸ�� ������ ���
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));

    // �ܰ� �ڽ� ����
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
