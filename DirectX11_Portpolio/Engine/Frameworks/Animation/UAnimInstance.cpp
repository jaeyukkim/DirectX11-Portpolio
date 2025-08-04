#include "HeaderCollection.h"
#include "UAnimInstance.h"

#include "Render/FSceneRender.h"
#include "Frameworks/Objects/Actor.h"

UAnimInstance::UAnimInstance(USkeletalMeshComponent* meshComp)
{
    Assert(meshComp != nullptr, "SkeletalMesh 가 nullptr 입니다.");
    MeshComponent = meshComp;
    
}

void UAnimInstance::InitInstance(Actor* InActorOwner, AnimInstanceCreateInfo info)
{
    ActorOwner = InActorOwner;
    
    shared_ptr<Converter> converter = make_shared<Converter>();
    
    wstring InFileName = L"../../Contents/_Models/" +
        String::ToWString(MeshComponent->MeshName) + L".model";
    
    converter->ReadBinary_Anim(InFileName, MeshComponent,
        FSceneRender::Get()->AnimProxyHasCreated(MeshComponent->MeshName));
    
    FSceneRender::Get()->CreateAnimRenderProxy<UAnimInstance>(MeshComponent->MeshName, this);

    InitAnimTable();
    NativeInitializeAnimation();
}

void UAnimInstance::NativeInitializeAnimation()
{
}

void UAnimInstance::AddNode(FAnimStateNode& InNode)
{
    AnimStateNode[InNode.NodeName] = InNode;
}

void UAnimInstance::InitFirstNode(const string& InName)
{
    if(AnimStateNode.find(InName) != AnimStateNode.end())
    {
        StartNode = &AnimStateNode[InName];
    }
}


void UAnimInstance::PlayAnimMontage(AnimMontage* montage)
{
    
    int clipID = GetAnimClipID(montage->AnimationName);
    Animations[clipID]->PlaySpeed = montage->PlaySpeed;

    RunningMontageData.Notifies.clear();
    for(FAnimationNotifyEvent* notify : montage->Triggers)
    {
        notify->bNotifyPlayed = false;
        RunningMontageData.Notifies.push_back(notify);
    }
    
    RunningMontageData.bMontagePlaying = true;
    RunningMontageData.MontageStartTime = Timer::Get()->GetRunningTime();
    RunningMontageData.MontageDuration = Animations[clipID]->Duration;
    RunningMontageData.MontageTickPerSeconds = Animations[clipID]->TickPerSecond;
    RunningMontageData.MontageSpeed = montage->PlaySpeed;

    
    ChangeAnimation(montage->AnimationName, montage->BlendTime, false);
}

void UAnimInstance::NativeUpdateAnimation(float deltaTime)
{
    CheckNull(StartNode);

    
    if (RunningMontageData.bMontagePlaying)
    {
        float running = Timer::Get()->GetRunningTime();
        float time = (running - RunningMontageData.MontageStartTime) * RunningMontageData.MontageSpeed;
        time = time * RunningMontageData.MontageTickPerSeconds;

        float progress = time / (RunningMontageData.MontageDuration/RunningMontageData.MontageSpeed) + 0.00001;
        if(time > RunningMontageData.MontageDuration)
        {
            RunningMontageData.bMontagePlaying = false;
        }

        for(FAnimationNotifyEvent* notify : RunningMontageData.Notifies)
        {
            if(!notify->bNotifyPlayed && progress >= notify->TriggerOnPercent)
            {
                notify->Trigger->Broadcast();
                notify->bNotifyPlayed = true;
            }
          
        }
        
    }
    else
    {
        FAnimStateNode* nextNode = ProcessNode(StartNode);
        ChangeAnimation(nextNode->NodeName, nextNode->TakeBlendTime, nextNode->bLoop);
    }
    
   
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
    }
    

    BlendChanged.Broadcast(MeshComponent->GetInstanceID(), BlendingData);
}

void UAnimInstance::ChangeAnimation(string InAnimName, float TakeTime, bool InbLoop)
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
    CheckTrue(((BlendingData.Current.ClipID == clipID) && (!RunningMontageData.bMontagePlaying)));
    CheckTrue(((BlendingData.Next.ClipID == clipID)  && (!RunningMontageData.bMontagePlaying)))

    BlendingData.ChangeStartTime = Timer::Get()->GetRunningTime();
    BlendingData.TakeTime = TakeTime;
    BlendingData.Next.ClipID = clipID;
    BlendingData.Next.Duration = Animations[clipID]->Duration + 1.0f;
    BlendingData.Next.TickPerSeconds = Animations[clipID]->TickPerSecond;
   
    BlendingData.Next.bLoop = (int)InbLoop;
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


FAnimStateNode* UAnimInstance::ProcessNode(FAnimStateNode* currentNode)
{
    // 현재 노드의 트랜지션을 검사
    for (auto& transition : currentNode->Transitions)
    {
        if (transition.Condition && transition.Condition())
        {
            if(AnimStateNode.find(transition.NextNodeName) != AnimStateNode.end())
                currentNode = ProcessNode(&AnimStateNode[transition.NextNodeName]);
        }
    }

    return currentNode;
}

