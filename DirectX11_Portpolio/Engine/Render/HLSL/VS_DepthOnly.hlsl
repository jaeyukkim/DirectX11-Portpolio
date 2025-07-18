#include "Common.hlsli"


//#ifdef SKINNED
cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;
};
//#endif


VertexOutput VS_Main(VertexShaderInput input)
{
    VertexOutput output;
    float4 posWorld;
    
    posWorld = mul(float4(input.posModel, 1.0f), World);
    output.posProj = mul(posWorld, ViewProjection);

    return output;


    
}