#include "Pch.h"
#include "AKachujin.h"
#include "Frameworks/UCameraComponent.h"

AKachujin::AKachujin()
{
    Mesh = make_shared<USkeletalMeshComponent>(L"Kachujin");
    Camera = make_shared<UCameraComponent>();


}

AKachujin::~AKachujin()
{
}

void AKachujin::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    Camera->TickComponent(deltaTime);
    Mesh->TickComponent(deltaTime);
}

void AKachujin::Render()
{
    Super::Render();

    Camera->RenderComponent();
    Mesh->RenderComponent();
}

