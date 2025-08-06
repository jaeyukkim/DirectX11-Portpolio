#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

//#define SKINNED 1 //디버그용
#define MAX_MODEL_TRANSFORM 250
#include "Material.hlsli"


cbuffer CB_World : register(b1)
{
    matrix World;
};

cbuffer CB_ViewContext : register(b2)
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix ProjectionInverse;
    matrix ViewProjection;
    float3 EyePos;
}

cbuffer CB_Time : register(b10)
{
    float DeltaTime = 0.0f;
    float RunningTime = 0.0f;
    bool bPaused = false;
    float CB_Time_padding = 0.0f;
}

struct VertexOutput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 modelNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR; // Normal lines 쉐이더에서 사용
    float3 tangent : TANGENT;
};

struct StaticMeshInput
{
    float4 posObject : POSITION; // 오브젝트의 정점 position
    float2 texCoord : TEXCOORD; // 텍스쳐 uv 좌표
    float4 ObjectColor : COLOR;
    float3 ObjectNormal : NORMAL; // 오브젝트의 정점의 normal    
    float3 tangent : TANGENT;
};


struct VertexShaderInput
{
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float2 texCoord : TEXCOORD; // <- 다음 예제에서 사용
    float4 modelColor : COLOR;
    float3 modelNormal : NORMAL; // 모델 좌표계의 normal    
    float3 tangent : TANGENT;

#ifdef SKINNED
    float4 blendIndicies : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHTS;
#endif
};


int RaySphereIntersection(in float3 start, in float3 dir, in float3 center, in float radius,
                            out float t1, out float t2)
{
    float3 p = start - center;
    float pdotv = dot(p, dir);
    float p2 = dot(p, p);
    float r2 = radius * radius;
    float m = pdotv * pdotv - (p2 - r2);
    
    if (m < 0.0)
    {
        t1 = 0;
        t2 = 0;
        return 0;
    }
    else
    {
        m = sqrt(m);
        t1 = -pdotv - m;
        t2 = -pdotv + m;
        return 1;
    }
}



struct FAnimFrameData
{
    int ClipID;

    float Duration;
    float TickPerSeconds;
    float FAPadding;

    float PlaySpeed;
    float StartTime;
    int bLoop;
    
    float afd_Padding2;
};

struct FAnimBlendingData
{
    float TakeTime;
    float ChangeStartTime;
    float2 bd_Padding;

    FAnimFrameData Current;
    FAnimFrameData Next;
};



struct InstanceData
{

    matrix Inst_World;
    float3 AABB_Max;
    float Inst_padding0;

    float3 AABB_Min;
    float Inst_padding1;

    int InstanceID;
    float3 Inst_padding2;

#ifdef SKINNED
    FAnimBlendingData BlendingData;
#endif
    
};


cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];
    matrix OffsetTransforms[MAX_MODEL_TRANSFORM];

};

cbuffer CB_BoneIndex : register(b11)
{
    uint BoneIndex;
    float3 Bone_pad;
};

StructuredBuffer<InstanceData> InstanceDatas : register(t25);



#ifdef SKINNED
Texture2DArray Animations : register(t26);

void CalcAnimFrameTime(in FAnimFrameData data, out int currentFrame, out int nextFrame, out float interpTime)
{
    float time = (RunningTime - data.StartTime) * data.PlaySpeed;
    time = time * data.TickPerSeconds;
    bool notLoop = (!data.bLoop && (time > data.Duration));
    
    if(!notLoop)
    {
        time = time % data.Duration;
        currentFrame = (int)time;
        nextFrame = (currentFrame + 1.0f) % data.Duration;
        interpTime = frac(time);
    }
    else
    {
        time = data.Duration-1;
        currentFrame = (int)time;
        nextFrame = currentFrame;
        interpTime = 0.0f;
    }
    
}

matrix GetAnimFrameTransform(int index, int currentFrame, int nextFrame, int clip, float interp)
{
    float4 m0 = Animations.Load(int4(index * 4 + 0, currentFrame, clip, 0));
    float4 m1 = Animations.Load(int4(index * 4 + 1, currentFrame, clip, 0));
    float4 m2 = Animations.Load(int4(index * 4 + 2, currentFrame, clip, 0));
    float4 m3 = Animations.Load(int4(index * 4 + 3, currentFrame, clip, 0));
    matrix curr = matrix(m0, m1, m2, m3);
        
    m0 = Animations.Load(int4(index * 4 + 0, nextFrame, clip, 0));
    m1 = Animations.Load(int4(index * 4 + 1, nextFrame, clip, 0));
    m2 = Animations.Load(int4(index * 4 + 2, nextFrame, clip, 0));
    m3 = Animations.Load(int4(index * 4 + 3, nextFrame, clip, 0));
    matrix next = matrix(m0, m1, m2, m3);
        
    return lerp(curr, next, interp);
}

float GetAnimClipChangingRatio(in FAnimBlendingData data)
{
    float time = (RunningTime - data.ChangeStartTime) / data.TakeTime;
    
    return time;
}

void SetAnimationWorld(inout matrix InWorld, VertexShaderInput input, FAnimBlendingData blendData)
{
    float indices[4] = { input.blendIndicies.x, input.blendIndicies.y, input.blendIndicies.z, input.blendIndicies.w };
    float weights[4] = { input.blendWeight.x, input.blendWeight.y, input.blendWeight.z, input.blendWeight.w };
    
    int clip[2], currFrame[2], nextFrame[2];
    float interp[2];

    clip[0] = blendData.Current.ClipID;
    clip[1] = blendData.Next.ClipID;
    

    CalcAnimFrameTime(blendData.Current, currFrame[0], nextFrame[0], interp[0]);


    matrix currClip = 0;
    matrix nextClip = 0;
    matrix transform = 0;

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        currClip = GetAnimFrameTransform(indices[i], currFrame[0], nextFrame[0], clip[0], interp[0]);

        [flatten]
        if (clip[1] > -1)
        {

            CalcAnimFrameTime(blendData.Next, currFrame[1], nextFrame[1], interp[1]);

            nextClip = GetAnimFrameTransform(indices[i], currFrame[1], nextFrame[1], clip[1], interp[1]);

            currClip = lerp(currClip, nextClip, GetAnimClipChangingRatio(blendData));
        }

        
        transform += mul(weights[i], currClip);
    }

    InWorld = mul(transform, InWorld);
    InWorld = mul(BoneTransforms[BoneIndex], InWorld);
}
#endif
#endif // __COMMON_HLSLI__
