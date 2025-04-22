#include "Common.hlsli"

#define MAX_MODEL_TRANSFORM 250


cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;

};


struct VertexShaderInput
{
    float4 posModel : POSITION; //모델 좌표계의 위치 position
    float2 texCoord : TEXCOORD; // <- 다음 예제에서 사용
    float4 modelColor : COLOR;
    float3 modelNormal : NORMAL; // 모델 좌표계의 normal    
    float3 tanzent : TANGENT;
    float4 blendIndicies : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHTS;

};

VertexOutput VS_Main(VertexShaderInput input)
{
    VertexOutput output;

   
    float4 posWorld = mul(input.posModel, World);
    output.posProj = mul(posWorld, ViewProjection);
    
    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)World));
    output.texCoord = input.texCoord;
    output.color = input.modelColor;

    return output;
}