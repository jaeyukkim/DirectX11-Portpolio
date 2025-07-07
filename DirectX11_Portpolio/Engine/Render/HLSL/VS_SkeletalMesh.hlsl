#include "Common.hlsli"

#define MAX_MODEL_TRANSFORM 250


cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;
};

struct SKM_Instance
{
    matrix Inst_World;
};

StructuredBuffer<SKM_Instance> InstanceData : register(t25);



VertexOutput VS_Main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;

    float newWorld = mul(BoneTransforms[BoneIndex], InstanceData[instanceID].Inst_World);

    float4 pos = float4(input.posModel, 1.0f);
    float4 posWorld = mul(pos, InstanceData[instanceID].Inst_World);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)InstanceData[instanceID].Inst_World));
    output.texCoord = input.texCoord;


    output.color = input.modelColor;
    output.tangent = mul(input.tangent, (float3x3) InstanceData[instanceID].Inst_World);

    return output;
}