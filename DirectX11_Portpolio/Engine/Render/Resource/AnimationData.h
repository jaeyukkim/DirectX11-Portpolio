#pragma once

#define MAX_MODEL_KEYFRAME 500

class Skeletal;

struct FAnimaFrameData
{
    int Clip = -1;
    float TickersPerSeconds;
    float Duration;
    UINT CurrentFrame = 0;
    
    UINT NextFrame = 0;
    float CurrentTime = 0.0f;
    float Speed = 1.0f;
    float Padding = 0.0f;;
};

struct FAnimBlendingData
{
    float TakeTime = 1.0f;
    float ChangingTime = 0.0f;
    float Speed = 1.0f;
    float Padding = 0.0f;

    FAnimaFrameData Current;
    FAnimaFrameData Next;
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


class FClipData
{
private:
    string Name;

    float Duration;
    float TickersPerSecond;

    vector<shared_ptr<FKeyFrameData>> Keyframes;

private:
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

    shared_ptr<ClipTransform> CalcClipTransform(const vector<shared_ptr<Skeletal>>& InBones);
    
private:
    friend class Converter;
    friend class AnimationTexture;
};

