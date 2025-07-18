#include "HeaderCollection.h"
#include "AnimationRenderProxy.h"

AnimationRenderProxy::AnimationRenderProxy(UAnimInstance* animInst)
    :RenderProxy(ERenderProxyType::PRT_Anim)
{
    AddInstance(animInst);
}

void AnimationRenderProxy::Render(const FRenderOption& option)
{
}

void AnimationRenderProxy::AddInstance(UAnimInstance* animInst)
{
    int instantID = animInst->MeshComponent->GetInstanceID();
    BlendingData[instantID] = animInst->BlendingData;
}

void AnimationRenderProxy::DeleteInstance(int InInstanceID)
{
    BlendingData.erase(InInstanceID);
}
