#include "Pch.h"
#include "Frameworks/UStaticMeshComponent.h"
#include "AFloor.h"
#include "Render/Material.h"


AFloor::AFloor()
{
	floor = CreateComponent<UStaticMeshComponent>(this, L"MapFloor");
	SetRootComponent(floor.get());
	
	
	
}

void AFloor::Tick(float deltaTime)
{
    
}

void AFloor::Render()
{
	floor->RenderComponent();
	
}

