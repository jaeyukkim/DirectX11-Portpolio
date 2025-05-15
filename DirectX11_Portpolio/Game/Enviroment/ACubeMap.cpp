#include "Pch.h"
#include "ACubeMap.h"
#include "Render/Material.h"
#include <Render/FSceneView.h>
#include "Frameworks/UStaticMeshComponent.h"


ACubeMap::ACubeMap()
{
    Sphere = CreateComponent<UStaticMeshComponent>(this, L"SkySphere");

    
    for (Material* mat : Sphere->GetAllMaterials())
    {
        mat->SetIsCubeMap(true);
        
        FSceneView::Get()->UpdateSkyLight(CubeMapType::ENVTEX, mat->GetSkyMapSRV(CubeMapType::ENVTEX));
        FSceneView::Get()->UpdateSkyLight(CubeMapType::SPECULAR, mat->GetSkyMapSRV(CubeMapType::SPECULAR));
        FSceneView::Get()->UpdateSkyLight(CubeMapType::IRRADIENCE, mat->GetSkyMapSRV(CubeMapType::IRRADIENCE));
        FSceneView::Get()->UpdateSkyLight(CubeMapType::BRDF, mat->GetSkyMapSRV(CubeMapType::BRDF));
    }

    SetRootComponent(Sphere.get());
   //FSceneView::Get()->UpdateIBLStrength(IBLStength);
   FSceneView::Get()->UpdateIBLStrength(0.0f);

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


