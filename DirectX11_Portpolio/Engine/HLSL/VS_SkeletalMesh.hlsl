#include "Common.hlsli"

#define MAX_MODEL_TRANSFORM 250


cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;
};




VertexOutput VS_Main(VertexShaderInput input)
{
    VertexOutput output;

    float newWorld = mul(BoneTransforms[BoneIndex], World);

    float4 pos = float4(input.posModel, 1.0f);
    float4 posWorld = mul(pos, World);
    output.posProj = mul(posWorld, ViewProjection);
    
    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)World));
    output.texCoord = input.texCoord;
    
    
    output.color = input.modelColor;
    output.tangent = mul(input.tangent, (float3x3) World);
    
    return output;
}