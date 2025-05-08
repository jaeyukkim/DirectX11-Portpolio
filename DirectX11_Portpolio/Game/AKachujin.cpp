#include "Pch.h"
#include "AKachujin.h"
#include "Frameworks/UCameraComponent.h"

AKachujin::AKachujin()
{
    Mesh = CreateComponent<USkeletalMeshComponent>(this, L"Kachujin", true);
    SetRootComponent(Mesh.get());

    Camera = CreateComponent<UCameraComponent>(this);
    Camera->GetRelativeTransform()->SetRotation(30.0f, 0.0f, 0.0f);
    Camera->GetRelativeTransform()->SetPosition(0.0f, 10.0f, -10.0f);
   // Camera->SetUpAttachment(GetRootComponent());
}

AKachujin::~AKachujin()
{
}

void AKachujin::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

}

void AKachujin::Render()
{
    Super::Render();
    Camera->RenderComponent();
    Mesh->RenderComponent();
}

