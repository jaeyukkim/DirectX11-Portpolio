#include "Pch.h"
#include "Frameworks/UStaticMeshComponent.h"
#include "AFloor.h"
#include "Render/Material.h"


AFloor::AFloor()
{
	floor = CreateComponent<UStaticMeshComponent>(this, L"MapFloor");
	SetRootComponent(floor.get());
	
	/*
	for (Material* grid : floor->GetAllMaterials())
	{
		grid->SetUVTiling(Vector2(5.0f, 5.0f));
	}
	*/
}

void AFloor::Tick(float deltaTime)
{
    /*
    if (ImGui::TreeNode("Material"))
    {

        int flag = 0;

        flag += ImGui::SliderFloat("Metallic", &Metallic, 0.0f, 1.0f);
        flag += ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f);
        flag += ImGui::CheckboxFlags("Use NormalMapping", &bUseNormalMap, 1);
        flag += ImGui::CheckboxFlags("Use MetallicMap", &bUseMetallicMap, 1);
        flag += ImGui::CheckboxFlags("Use RoughnessMap", &bUseRoughnessMap, 1);


        if (flag)
        {
            for (Material* mat : floor->GetAllMaterials())
            {
                Material::MaterialDescription* desc = mat->GetMatDesc();
                desc->Metallic = Metallic;
                desc->Roughness = Roughness;
                desc->bUseAlbedoMap = bUseAlbedoMap;
                desc->bUseNormalMap = bUseNormalMap;
                desc->bUseMetallicMap = bUseMetallicMap;
                desc->bUseRoughnessMap = bUseRoughnessMap;
                desc->bInvertNormalMapY = true;
            }
        }

        ImGui::TreePop();
    }
    */
}

void AFloor::Render()
{
	floor->RenderComponent();
}

