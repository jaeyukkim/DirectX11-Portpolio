#pragma once
#include "Render/Resource/AnimationData.h"


class FClipData;

struct AnimInstanceCreateInfo
{
    vector<string> exportAnimName;

};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnBlendDataChanged, int, FAnimBlendingData&);

class UAnimInstance
{
public:
    UAnimInstance(USkeletalMeshComponent* meshComp);
    virtual ~UAnimInstance() = default;
    
    virtual void NativeUpdateAnimation(float deltaTime);
    virtual void ChangeAnimation(string InAnimName, float TakeTime);
    virtual void InitInstance(AnimInstanceCreateInfo info = AnimInstanceCreateInfo());
    
    int GetAnimClipID(string InAnimName);
    
private:
    void InitAnimTable();
    void SetAnimInst(const vector<shared_ptr<FClipData>>& InAnimations) {Animations = InAnimations;}
private:
    USkeletalMeshComponent* MeshComponent;
    vector<shared_ptr<FClipData>> Animations;

    
private:
    ComPtr<ID3D11Texture2D> ClipTexture = nullptr;
    ComPtr<ID3D11ShaderResourceView> ClipsSRV = nullptr;
    FAnimBlendingData BlendingData;
    unordered_map<string, int> AnimClipTable;


private:
    FOnBlendDataChanged BlendChanged;
    bool bAnimStateChanged = false;

private:
    friend class AnimationRenderProxy;
    friend class AnimationTexture;
    friend class Converter;
};
