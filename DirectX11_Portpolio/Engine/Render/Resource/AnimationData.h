#pragma once


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
    friend class Converter;
    
};

