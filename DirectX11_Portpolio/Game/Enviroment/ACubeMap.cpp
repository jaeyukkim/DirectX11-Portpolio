#include "Pch.h"
#include "ACubeMap.h"
#include <Render/Resource/Material.h>
#include <Render/Resource/FSceneView.h>
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "Render/FSceneRender.h"


ACubeMap::ACubeMap()
{
    StaticMeshCreateInfo info;
    info.bIsSkyBox = true;
    
    Sphere = CreateComponent<UStaticMeshComponent>(this, L"SkySphere", info);
    FSceneRender::Get()->CreateRenderProxy<SkyBoxRenderProxy>(Sphere.get());
    
    for (Material* mat : Sphere->GetAllMaterials())
    {
        mat->SetIsCubeMap(true);
        
        FSceneView::Get()->UpdateSkyLight(ECubeMapType::ENVTEX, mat->GetSkyMapSRV(ECubeMapType::ENVTEX));
        FSceneView::Get()->UpdateSkyLight(ECubeMapType::SPECULAR, mat->GetSkyMapSRV(ECubeMapType::SPECULAR));
        FSceneView::Get()->UpdateSkyLight(ECubeMapType::IRRADIENCE, mat->GetSkyMapSRV(ECubeMapType::IRRADIENCE));
        FSceneView::Get()->UpdateSkyLight(ECubeMapType::BRDF, mat->GetSkyMapSRV(ECubeMapType::BRDF));
    }

    SetRootComponent(Sphere.get());
    FSceneView::Get()->UpdateIBLStrength(IBLStength);
    
}

void ACubeMap::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

