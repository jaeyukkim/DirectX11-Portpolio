#include "Common.hlsli"




VertexOutput VS_Main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;

    
    float4 posWorld = mul(float4(input.posModel, 1.0f), InstanceDatas[instanceID].Inst_World);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.modelNormal, (float3x3)InstanceDatas[instanceID].Inst_World));

    output.texCoord = input.texCoord;
    output.color = input.modelColor;
    output.posModel = input.posModel;

    float4 tangentWorld = float4(input.tangent, 0.0f);
    tangentWorld = mul(tangentWorld, InstanceDatas[instanceID].Inst_World);
    output.tangent = tangentWorld.xyz;


    return output;
}