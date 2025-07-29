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