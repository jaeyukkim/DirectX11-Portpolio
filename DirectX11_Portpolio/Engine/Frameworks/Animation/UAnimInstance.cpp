#include "HeaderCollection.h"
#include "UAnimInstance.h"

#include "Render/FSceneRender.h"

UAnimInstance::UAnimInstance(USkeletalMeshComponent* meshComp)
{
    Assert(meshComp != nullptr, "SkeletalMesh 가 nullptr 입니다.");
    MeshComponent = meshComp;
    
}

void UAnimInstance::InitInstance(AnimInstanceCreateInfo info)
{
    shared_ptr<Converter> converter = make_shared<Converter>();
    
    wstring InFileName = L"../../Contents/_Models/" +
        String::ToWString(MeshComponent->MeshName) + L".model";
    
    converter->ReadBinary_Anim(InFileName, MeshComponent,
        FSceneRender::Get()->AnimProxyHasCreated(MeshComponent->MeshName));
    
    FSceneRender::Get()->CreateAnimRenderProxy<UAnimInstance>(MeshComponent->MeshName, this);

    InitAnimTable();
}


void UAnimInstance::NativeUpdateAnimation(float deltaTime)
{
    if(BlendingData.Next.ClipID > -1)
    {
        float running = Timer::Get()->GetRunningTime();
        float current = BlendingData.ChangeStartTime;
        float take = BlendingData.TakeTime;

        // (running - current)는 블렌딩이 시작된 후 경과된 시간
        // take는 블랜드에 걸릴 총 시간
        float time = (running - current) / take;

        
        if (time >= 1.0f)
        {
            BlendingData.ChangeStartTime = 0.0f;
            BlendingData.TakeTime = 0.0f;
            BlendingData.Current = BlendingData.Next;
            BlendingData.Next.ClipID = -1;
            bAnimStateChanged = false;
        }
        
        // else 인 경우는 현재 블렌딩 중이며 아직 완료되지 않은 상태.
    }

    BlendChanged.Broadcast(MeshComponent->GetInstanceID(), BlendingData);
}

void UAnimInstance::ChangeAnimation(string InAnimName, float TakeTime)
{
    int clipID = GetAnimClipID(InAnimName);
    
    if (BlendingData.Current.ClipID < 0) //최초 실행 일 때
    {
        BlendingData.Current.ClipID = clipID;

        BlendingData.Current.Duration = Animations[clipID]->Duration + 1.0f;
        BlendingData.Current.TickPerSeconds = Animations[clipID]->TickPerSecond;

        BlendingData.Current.PlaySpeed = Animations[clipID]->PlaySpeed;
        BlendingData.Current.StartTime = Timer::Get()->GetRunningTime();

        bAnimStateChanged = true;
        return;
    }

    //같은 애니메이션이 들어왔을경우
    CheckTrue(BlendingData.Current.ClipID == clipID);


    BlendingData.ChangeStartTime = Timer::Get()->GetRunningTime();
    BlendingData.TakeTime = TakeTime;
    BlendingData.Next.ClipID = clipID;
    BlendingData.Next.Duration = Animations[clipID]->Duration + 1.0f;
    BlendingData.Next.TickPerSeconds = Animations[clipID]->TickPerSecond;

    
    BlendingData.Next.PlaySpeed = Animations[clipID]->PlaySpeed;
    BlendingData.Next.StartTime = Timer::Get()->GetRunningTime();
    bAnimStateChanged = true;
}



int UAnimInstance::GetAnimClipID(string InAnimName)
{
    if(AnimClipTable.find(InAnimName) != AnimClipTable.end())
    {
        return AnimClipTable[InAnimName];
    }

    return -1;
}


/**
 * Animations의 ClipID와 애니메이션 이름을 매핑하는 함수
 */
void UAnimInstance::InitAnimTable()
{
    for(int i = 0 ; i<Animations.size() ; i++)
    {
        AnimClipTable.insert({Animations[i]->AnimName, i});
    }
}

