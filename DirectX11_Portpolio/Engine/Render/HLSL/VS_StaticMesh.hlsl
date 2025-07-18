#include "Common.hlsli"




StructuredBuffer<SM_Instance> InstanceData : register(t25);


VertexOutput VS_Main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;

    
    float4 posWorld = mul(float4(input.posModel, 1.0f), InstanceData[instanceID].Inst_World);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)InstanceData[instanceID].Inst_World));

    output.texCoord = input.texCoord;
    output.color = input.modelColor;


    float4 tangentWorld = float4(input.tangent, 0.0f);
    tangentWorld = mul(tangentWorld, InstanceData[instanceID].Inst_World);
    output.tangent = tangentWorld.xyz;


    return output;
}