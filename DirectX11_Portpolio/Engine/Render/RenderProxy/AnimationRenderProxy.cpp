#include "HeaderCollection.h"
#include "AnimationRenderProxy.h"

AnimationRenderProxy::AnimationRenderProxy(UAnimInstance* animInst)
    :RenderProxy(ERenderProxyType::PRT_Anim)
{
    ClipsSRV = std::move(animInst->ClipsSRV);
    ClipTexture = std::move(animInst->ClipTexture);
    Animations = animInst->Animations;
    AddInstance(animInst);
}


void AnimationRenderProxy::Render(const FRenderOption& option)
{

    D3D::Get()->GetDeviceContext()->VSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::ERS_AnimationData),
        1, ClipsSRV.GetAddressOf());
}

void AnimationRenderProxy::AddInstance(UAnimInstance* animInst)
{
    int instantID = animInst->MeshComponent->GetInstanceID();
    BlendingData[instantID] = animInst->BlendingData;
    animInst->SetAnimInst(Animations);
    animInst->BlendChanged.Add(this, &AnimationRenderProxy::SetBlendData);
}

void AnimationRenderProxy::DeleteInstance(int InInstanceID)
{
    BlendingData.erase(InInstanceID);
}

void AnimationRenderProxy::SetBlendData(int InInstanceID, FAnimBlendingData& data)
{
    BlendingData[InInstanceID] = data;
    BlendDataUpdate.Broadcast(InInstanceID, data);
}
