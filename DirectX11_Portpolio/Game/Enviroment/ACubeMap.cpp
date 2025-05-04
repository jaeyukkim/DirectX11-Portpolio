#include "Pch.h"
#include "ACubeMap.h"
#include "Render/Material.h"
#include "Frameworks/UStaticMeshComponent.h"

ACubeMap::ACubeMap()
{
    Sphere = CreateComponent<UStaticMeshComponent>(this, L"SkySphere");

    for (Material* mat : Sphere->GetAllMaterials())
    {
        mat->SetIsCubeMap(true);
    }
    SetRootComponent(Sphere.get());
   
}

void ACubeMap::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void ACubeMap::Render()
{
    Super::Render();

   
    Sphere->RenderComponent();

}


