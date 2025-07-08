#include "Common.hlsli"

cbuffer CB_Frustum : register(b9)
{
    float4 FrustumPlane[6];
};

ConsumeStructuredBuffer<SM_Instance> InInstance : register(u0);
AppendStructuredBuffer<SM_Instance> OutInstance : register(u1);

[numthreads(1, 1, 1)]
void CS_Main( uint3 DTid : SV_DispatchThreadID )
{
    
}