#pragma once

#define MAX_MODEL_KEYFRAME 800

class Skeletal;

struct FAnimFrameData
{
    int ClipID = -1;

    float Duration = 0.0f;
    float TickPerSeconds = 0.0f;
    float Padding = 0.0f;

    float PlaySpeed = 0.0f;
    float StartTime = 0.0f;

    int bLoop = true;
    float Padding2 = 0.0f;
};

struct FAnimBlendingData
{
    float TakeTime = 1.0f;
    float ChangeStartTime = 0.0f;
    float Padding[2] = {0.0f};

    FAnimFrameData Current;
    FAnimFrameData Next;
};

template<typename ValType>
struct FFrameData
{
public:
    float mTime;
    ValType mValue;

    FFrameData()
    {
        mTime = 0.0f;
        mValue = ValType();
    }
    FFrameData(float InTime, ValType val)
    {
        mTime = InTime;
        mValue = val;
    }
};


struct FKeyFrameData
{
    int BoneIndex = -1;
    string BoneName = "";
		
    vector<FFrameData<Vector3>> Positions;
    vector<FFrameData<Quaternion>> Rotations;
    vector<FFrameData<Vector3>> Scalings;
};

struct ClipTransform
{
    Matrix** Transform;

    ClipTransform()
    {
        Transform = new Matrix*[MAX_MODEL_KEYFRAME];

        for (UINT i = 0; i < MAX_MODEL_KEYFRAME; i++)
            Transform[i] = new Matrix[MAX_MODEL_TRANSFORM];
    }

    ~ClipTransform()
    {
        for (UINT i = 0; i < MAX_MODEL_KEYFRAME; i++)
            DeleteArray(Transform[i]);

        DeleteArray(Transform);
    }
};


struct FAnimationNotifyEvent
{
public:
    FAnimationNotifyEvent() 
        : Trigger(nullptr), TriggerOnPercent(0.0f), bNotifyPlayed(false) { }
    FAnimationNotifyEvent(FDynamicDelegate<>* InTrigger, float InTriggerTime)
        : Trigger(InTrigger), TriggerOnPercent(InTriggerTime) { }
    
    float TriggerOnPercent = 0.0f;
    FDynamicDelegate<>* Trigger;
    bool bNotifyPlayed = false;
};

class AnimMontage
{
public:
    AnimMontage(string InAnimationName) { AnimationName = InAnimationName;}
    string AnimationName;
    float BlendTime = 0.1f;
    float PlaySpeed = 1.0f;
    vector<FAnimationNotifyEvent*> Triggers;
};

class FClipData
{
public:
    string AnimName;
    
    float Duration;
    float TickPerSecond;
    float PlaySpeed = 1.0f;

    void ClearKeyFrame();

private:
    vector<shared_ptr<FKeyFrameData>> Keyframes;
    shared_ptr<ClipTransform> CalcClipTransform(const vector<shared_ptr<Skeletal>>& InBones);
    
private:
    friend class Converter;
    friend class AnimationTexture;
};


struct FAnimTransition
{
    string NextNodeName;
    std::function<bool()> Condition;
};


struct FAnimStateNode
{
    string NodeName; //노드 이름
    float TakeBlendTime;
    bool bLoop = true;
    vector<FAnimTransition> Transitions;   // 이 노드에서 갈 수 있는 트랜지션들
};

struct MontageData
{
    bool bMontagePlaying = false;
    float MontageStartTime = 0.0f;
    float MontageDuration = 0.0f;
    float MontageTickPerSeconds = 0.0f;
    float MontageSpeed = 1.0f;
    vector<FAnimationNotifyEvent*> Notifies;
};