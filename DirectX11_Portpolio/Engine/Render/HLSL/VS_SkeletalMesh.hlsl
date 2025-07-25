#include "Common.hlsli"




VertexOutput VS_Main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;

    matrix newWorld = InstanceDatas[instanceID].Inst_World;
    
#ifdef SKINNED
    SetAnimationWorld(newWorld, input, InstanceDatas[instanceID].BlendingData);
#endif

    float4 posWorld = mul(float4(input.posModel, 1.0f), newWorld);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)newWorld));
    output.texCoord = input.texCoord;


    output.color = input.modelColor;
    output.tangent = mul(input.tangent, (float3x3)newWorld);

    return output;
}