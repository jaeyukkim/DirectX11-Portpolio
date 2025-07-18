#pragma once
#include "Render/Resource/AnimationData.h"


class FClipData;

struct AnimInstanceCreateInfo
{
    vector<string> exportAnimName;
    bool bReadAnimation = true;
    bool bExportAnimation = false;
};


class UAnimInstance
{
public:
    UAnimInstance(USkeletalMeshComponent* meshComp,
        AnimInstanceCreateInfo info = AnimInstanceCreateInfo());
    virtual ~UAnimInstance() = default;
    
    virtual void NativeUpdateAnimation(float deltaTime);
public:


    
private:
    USkeletalMeshComponent* MeshComponent;
    vector<shared_ptr<FClipData>> Animations;

    
private:
    ComPtr<ID3D11ShaderResourceView> ClipsSRV = nullptr;
    FAnimBlendingData BlendingData;



private:
    friend class AnimationRenderProxy;
    friend class AnimationTexture;
    friend class Converter;
};
