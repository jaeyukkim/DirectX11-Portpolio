#pragma once
#include "RenderProxy.h"

class AnimationRenderProxy : public RenderProxy
{
public:
    AnimationRenderProxy(UAnimInstance* animInst);
    void Render(const FRenderOption& option) override;

    void AddInstance(UAnimInstance* animInst);
    void DeleteInstance(int InInstanceID);
   
    unordered_map<int, FAnimBlendingData> BlendingData;
};
