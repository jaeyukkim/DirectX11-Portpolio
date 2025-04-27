#include "Pch.h"
#include "Frameworks/UStaticMeshComponent.h"
#include "AFloor.h"
#include "Render/Material.h"


AFloor::AFloor()
{
	floor = CreateComponent<UStaticMeshComponent>(this, L"Floor");
	SetRootComponent(floor.get());
	
	Material* grid = floor->GetMaterial("WorldGridMaterial");
	CheckNull(grid);
	
	grid->SetUVTiling(Vector2(10.0f, 10.0f));

	
}

void AFloor::Tick(float deltaTime)
{
}

void AFloor::Render()
{
	floor->RenderComponent();
}

