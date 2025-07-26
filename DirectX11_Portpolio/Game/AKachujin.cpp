#include "Pch.h"
#include "Frameworks/Camera/UCameraComponent.h"
#include "Frameworks/Animation/UAnimInstance.h"
#include "AKachujin.h"

AKachujin::AKachujin()
{
    /*
    shared_ptr<Converter> converter = make_shared<Converter>();
    converter->ImportFBXFile(L"Paladin", EMeshType::SkeletalMeshType);

    
    string animName[4] =
    {
        "sword_and_shield_idle",
        "sword_and_shield_walk",
        "sword_and_shield_jump",
        "sheath_sword_1"
    };
    for(string str : animName)
    {
        converter->ImportFBX_Animation(L"Paladin", String::ToWString(str));
    }
    */
    
    Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"Paladin"));
    Mesh[0]->CreateAnimInstance<UAnimInstance>();
    

    /*
    shared_ptr<Converter> converter = make_shared<Converter>();
    converter->ImportFBXFile(L"SunGlassGirl", EMeshType::SkeletalMeshType);
    
    
    string animName[4] =
    {
        "CatwalkIdle",
        "CatwalkIdleToWalkForward",
        "CatwalkWalkForward",
        "CatwalkWalkStop"
    };
    for(string str : animName)
    {
        converter->ImportFBX_Animation(L"SunGlassGirl", String::ToWString(str));
    }
    
    
    Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"SunGlassGirl"));
    Mesh[0]->CreateAnimInstance<UAnimInstance>();
    */
    
    //Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"Paladin"));
    //Mesh[1]->CreateAnimInstance<UAnimInstance>();
    //Mesh[1]->GetRelativeTransform()->SetPosition(Vector3(15.0f, 0.75f, 6.0f));



    /*
    string animName[4] =
    {
        "sword_and_shield_idle",
        "sword_and_shield_walk",
        "sword_and_shield_jump",
        "sheath_sword_1"
    };
    int cnt = 0;
    for(int i = 0 ; i<30 ; i++)
    {
        for(int j = 0 ; j<30 ; j++)
        {
            Mesh.push_back(CreateComponent<USkeletalMeshComponent>(this, L"Paladin"));
            Mesh[cnt]->CreateAnimInstance<UAnimInstance>();
            Mesh[cnt]->GetRelativeTransform()->SetPosition(Vector3(i*4, 0.75f, j*4));
            Mesh[cnt]->GetAnimInstance()->ChangeAnimation(animName[cnt%4], 0.15f);

            cnt++;
        }
    }
    */
    Mesh[0]->SetUpAttachment(GetRootComponent());

    

    Camera = CreateComponent<UCameraComponent>(this);
    Camera->SetUpAttachment(GetRootComponent());
    Camera->GetRelativeTransform()->SetPosition(0.0f, 200.0f, -300.0f);
    Camera->GetRelativeTransform()->SetRotation(20.0f, 0.0f, 0.0f);
}

AKachujin::~AKachujin()
{
}

void AKachujin::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    
    if(Keyboard::Get()->Down('Q'))
        Mesh[0]->GetAnimInstance()->ChangeAnimation("sword_and_shield_idle", 0.15f);
    if(Keyboard::Get()->Down('E'))
        Mesh[0]->GetAnimInstance()->ChangeAnimation("sword_and_shield_walk", 0.15f);
    

    /*
    if(Keyboard::Get()->Down('Q'))
        Mesh[0]->GetAnimInstance()->ChangeAnimation("CatwalkIdle", 1);
    if(Keyboard::Get()->Down('E'))
        Mesh[0]->GetAnimInstance()->ChangeAnimation("CatwalkWalkForward", 1);
    */
    
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