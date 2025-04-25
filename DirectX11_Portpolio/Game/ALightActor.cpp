#include "Pch.h"
#include "Light/ULightComponent.h"
#include "ALightActor.h"

#include "Frameworks/UStaticMeshComponent.h"

ALightActor::ALightActor()
{
	Mesh = CreateComponent<UStaticMeshComponent>(this, L"Lantern");
	SetRootComponent(Mesh.get());
	LightComp = CreateComponent<ULightComponent>(this, ELightType::Spot);
	LightComp->SetUpAttachment(GetRootComponent());
	DirectionalLightComp = CreateComponent<ULightComponent>(this, ELightType::Directional);
	
	
}

ALightActor::~ALightActor()
{
}

void ALightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ImGui::SliderFloat3("Light Position", &GetActorTransform()->Position.x, -20.0f, 20.0f);
	Vector3 Rotation = GetActorTransform()->GetRotation();
	ImGui::SliderFloat3("Light direction", &Rotation.x, -180.0f, 180.0f);
	GetActorTransform()->SetRotation(Rotation);

	ImGui::SliderFloat4("Light Color", &LightComp->GetLightInfo()->color.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Light fallOffStart", &LightComp->GetLightInfo()->fallOffStart, 0.0f, 5.0f);
	ImGui::SliderFloat("Light fallOffEnd", &LightComp->GetLightInfo()->fallOffEnd, 0.0f, 100.0f);
	ImGui::SliderFloat("Light spotPower", &LightComp->GetLightInfo()->spotPower, 0.0f, 20.0f);
	ImGui::SliderFloat3("Light Strengh", &LightComp->GetLightInfo()->strength.x, 0.0f, 100.0f);

}

void ALightActor::Render()
{
	Super::Render();

	DirectionalLightComp->RenderComponent();
	LightComp->RenderComponent();
	Mesh->RenderComponent();
	
}
