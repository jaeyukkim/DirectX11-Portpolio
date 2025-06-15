#include "Pch.h"
#include "Frameworks/Camera/UCameraComponent.h"
#include "AKachujin.h"

AKachujin::AKachujin()
{
    //Mesh = CreateComponent<USkeletalMeshComponent>(this, L"Samurai", true);
    //SetRootComponent(Mesh.get());

    Camera = CreateComponent<UCameraComponent>(this);
    Camera->GetRelativeTransform()->SetRotation(30.0f, 0.0f, 0.0f);
    Camera->GetRelativeTransform()->SetPosition(0.0f, 3.0f, -3.0f);
   // Camera->SetUpAttachment(GetRootComponent());
}

AKachujin::~AKachujin()
{
}

void AKachujin::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    
    /*
    if (ImGui::TreeNode("ModelMaterial")) 
    {

        int flag = 0;

        flag += ImGui::SliderFloat("Metallic", &Metallic, 0.0f, 1.0f);
        flag += ImGui::SliderFloat("Roughness", &Roughness,0.0f, 1.0f);
        flag += ImGui::CheckboxFlags("AlbedoTexture", &bUseAlbedoMap, 1);
        flag += ImGui::CheckboxFlags("EmissiveTexture", &bUseEmissiveMap, 1);
        flag += ImGui::CheckboxFlags("Use NormalMapping", &bUseNormalMap, 1);
        flag += ImGui::CheckboxFlags("Use AO", &bUseAOMap, 1);
        flag += ImGui::CheckboxFlags("Use MetallicMap", &bUseMetallicMap, 1);
        flag += ImGui::CheckboxFlags("Use RoughnessMap",&bUseRoughnessMap, 1);
       

        if (flag) 
        {
            for (Material* mat : Mesh->GetAllMaterials())
            {
                Material::MaterialDescription* desc = mat->GetMatDesc();
                desc->Metallic = Metallic;
                desc->Roughness = Roughness;
                desc->bUseAlbedoMap = bUseAlbedoMap;
                desc->bUseEmissiveMap = bUseEmissiveMap;
                desc->bUseNormalMap = bUseNormalMap;
                desc->bUseAOMap = bUseAOMap;
                desc->bUseMetallicMap = bUseMetallicMap;
                desc->bUseRoughnessMap = bUseRoughnessMap;
                desc->bInvertNormalMapY = true;
            }
        }

        ImGui::TreePop();
    }
    
    */
}