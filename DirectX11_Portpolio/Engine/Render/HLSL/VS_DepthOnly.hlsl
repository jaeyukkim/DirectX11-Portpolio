#include "Common.hlsli"

#ifndef SKINNED

VertexOutput VS_Main(VertexShaderInput input)
{
    VertexOutput output;
    float4 posWorld;
    
    posWorld = mul(float4(input.posModel, 1.0f), World);
    output.posProj = mul(posWorld, ViewProjection);

    return output;
}

#endif



#ifdef SKINNED

VertexOutput VS_Main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;

    matrix newWorld = InstanceDatas[instanceID].Inst_World;
    SetAnimationWorld(newWorld, input, InstanceDatas[instanceID].BlendingData);
    
    float4 posWorld = mul(float4(input.posModel, 1.0f), newWorld);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)InstanceDatas[instanceID].Inst_World));
    output.texCoord = input.texCoord;



    output.tangent = mul(input.tangent, (float3x3)newWorld);

    return output;
}

#endif

