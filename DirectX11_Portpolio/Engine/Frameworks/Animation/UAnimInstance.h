#pragma once
#include "Render/Resource/AnimationData.h"


class Actor;
class USkeletalMeshComponent;
class ACharacter;
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

    virtual void NativeInitializeAnimation();
    virtual void NativeUpdateAnimation(float deltaTime);
    virtual void ChangeAnimation(string InAnimName, float TakeTime, bool InbLoop);
    virtual void InitInstance(Actor* InActorOwner, AnimInstanceCreateInfo info = AnimInstanceCreateInfo());
    virtual void PlayAnimMontage(AnimMontage* montage);
    int GetAnimClipID(string InAnimName);

    
protected:
    Actor* GetActorOwner() {return ActorOwner;}
    virtual void InitFirstNode(const string& InName);
    virtual void AddNode(FAnimStateNode& InNode);

    
private:
    void InitAnimTable();
    void SetAnimInst(const vector<shared_ptr<FClipData>>& InAnimations) {Animations = InAnimations;}
    FAnimStateNode* ProcessNode(FAnimStateNode* currentNode);
    
private:
    USkeletalMeshComponent* MeshComponent;
    
    vector<shared_ptr<FClipData>> Animations;
    ComPtr<ID3D11Texture2D> ClipTexture = nullptr;
    ComPtr<ID3D11ShaderResourceView> ClipsSRV = nullptr;

private:
    FAnimBlendingData BlendingData;
    unordered_map<string, int> AnimClipTable;
    
    unordered_map<string, FAnimStateNode> AnimStateNode;
    FAnimStateNode* StartNode = nullptr;
    
    FOnBlendDataChanged BlendChanged;
    bool bAnimStateChanged = false;
    MontageData RunningMontageData;
    
private:
    Actor* ActorOwner;
    
private:
    friend class AnimationRenderProxy;
    friend class AnimationTexture;
    friend class Converter;
};
