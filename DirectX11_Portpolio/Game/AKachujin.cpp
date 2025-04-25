#include "Pch.h"
#include "AKachujin.h"
#include "Frameworks/UCameraComponent.h"

AKachujin::AKachujin()
{
    Mesh = CreateComponent<USkeletalMeshComponent>(this, L"Kachujin");
    SetRootComponent(Mesh.get());

    Camera = CreateComponent<UCameraComponent>(this);
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

