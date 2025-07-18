#include "HeaderCollection.h"
#include "UAnimInstance.h"

#include "Render/FSceneRender.h"

UAnimInstance::UAnimInstance(USkeletalMeshComponent* meshComp, AnimInstanceCreateInfo info)
{
    Assert(meshComp != nullptr, "SkeletalMesh 가 nullptr 입니다.");
    MeshComponent = meshComp;

    
    shared_ptr<Converter> converter = make_shared<Converter>();
    
    if(info.bExportAnimation)
    {
        for(string animName : info.exportAnimName)
        {
            converter->ImportFBX_Animation(String::ToWString(MeshComponent->MeshName), String::ToWString(animName));
        }
    }

    if(info.bReadAnimation)
    {
        wstring InFileName = L"../../Contents/_Models/" +
            String::ToWString(MeshComponent->MeshName) + L".model";
        
        converter->ReadBinary_Anim(InFileName, MeshComponent,
        FSceneRender::Get()->SkeletalMeshHasCreated(MeshComponent->MeshName));
    }
}

void UAnimInstance::NativeUpdateAnimation(float deltaTime)
{
    
}