#include "Pch.h"
#include "AFloor.h"
#include "Frameworks/Components/UStaticMeshComponent.h"

#include "Frameworks/Collision/CollisionDefinition.h"
#include "Frameworks/Collision/UBoxComponent.h"


AFloor::AFloor()
{
	boxCollision = CreateComponent<UBoxComponent>(this, Vector3(10000.0f, 145.0f, 10000.0f));
	SetRootComponent(boxCollision.get());
	floor = CreateComponent<UStaticMeshComponent>(this, L"MapFloor");
	floor->SetUpAttachment(GetRootComponent());
}

void AFloor::Tick(float deltaTime)
{
    
}

