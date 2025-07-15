#include "Pch.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "AFloor.h"



AFloor::AFloor()
{


	floor = CreateComponent<UStaticMeshComponent>(this, L"MapFloor");
	SetRootComponent(floor.get());
	
	
	
}

void AFloor::Tick(float deltaTime)
{
    
}

