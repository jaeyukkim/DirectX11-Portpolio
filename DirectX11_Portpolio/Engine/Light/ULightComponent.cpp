#include "HeaderCollection.h"
#include "Render/FSceneView.h"
#include "ULightComponent.h"

ULightComponent::ULightComponent(ELightType InLightType)
{
    LightInfo.LightType = InLightType;
    FSceneView::Get()->AddToLightMap(&LightInfo);
}

ULightComponent::~ULightComponent()
{
}

void ULightComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);


    FTransform* transform = GetTransform();
    

    // 임시로 추가한것들임.
    ImGui::SliderFloat3("Light Position", &transform->Position.x, -50.0f, 50.0f);
    ImGui::SliderFloat3("Light direction", &transform->Rotation.x, -180.0f, 180.0f);

    ImGui::SliderFloat4("Light Color", &LightInfo.color.x, 0.0f, 1.0f);
    ImGui::SliderFloat("Light fallOffStart", &LightInfo.fallOffStart, 0.0f, 5.0f);
    ImGui::SliderFloat("Light fallOffEnd", &LightInfo.fallOffEnd, 0.0f, 100.0f);
    ImGui::SliderFloat("Light spotPower", &LightInfo.spotPower, 1.0f, 20.0f);
    ImGui::SliderFloat3("Light Strengh", &LightInfo.strength.x, 0.0f, 100.0f);

    LightInfo.direction = Vector3(transform->GetRotation());
    LightInfo.position = transform->GetPosition();


    FSceneView::Get()->UpdateLightMap(LightInfo);
}

void ULightComponent::RenderComponent()
{
	Super::RenderComponent();
}

 
