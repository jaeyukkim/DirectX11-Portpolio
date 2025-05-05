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

	LightComp->CachedEulerRotation = GetActorTransform()->GetRotation();
	//DirectionalLightComp->GetLightInfo()->strength = Vector3(1.5f, 1.5f, 1.5f);


}

ALightActor::~ALightActor()
{
}

void ALightActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ImGui::SliderFloat3("Light Position", &GetActorTransform()->Position.x, -20.0f, 20.0f);

	ImGui::SliderFloat3("Light Rotation", &LightComp->CachedEulerRotation.x, -180.0f, 180.0f);
	GetActorTransform()->SetRotation(LightComp->CachedEulerRotation);

	ImGui::SliderFloat("Light fallOffStart", &LightComp->GetLightInfo()->fallOffStart, 0.0f, 5.0f);
	ImGui::SliderFloat("Light fallOffEnd", &LightComp->GetLightInfo()->fallOffEnd, 0.0f, 100.0f);
	ImGui::SliderFloat("Light spotPower", &LightComp->GetLightInfo()->spotPower, 1.0f, 100.0f);
	ImGui::SliderFloat3("Light Strengh", &LightComp->GetLightInfo()->strength.x, 0.0f, 100.0f);

}

void ALightActor::Render()
{
	Super::Render();

	DirectionalLightComp->RenderComponent();
	LightComp->RenderComponent();
	Mesh->RenderComponent();
	
}
