#include "Pch.h"
#include "Frameworks/Camera/UCameraComponent.h"
#include "AKachujin.h"

AKachujin::AKachujin()
{
    //Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"Samurai"));
    //Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"Samurai"));

    //Mesh[1]->GetRelativeTransform()->SetPosition(Vector3(15.0f, 0.25f, 6.0f));

    /*
    int cnt = 0;
    for(int i = 0 ; i<15 ; i++)
    {
        for(int j = 0 ; j<15 ; j++)
        {
            Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"Samurai"));
            Mesh[cnt]->GetRelativeTransform()->SetPosition(Vector3(i*4, 0.5f, j*4));
            cnt++;
        }
    }
    */
    //SetRootComponent(Mesh[0].get());

    

    Camera = CreateComponent<UCameraComponent>(this);
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