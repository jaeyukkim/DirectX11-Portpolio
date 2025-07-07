#include "Common.hlsli"


struct SM_Instance
{
    matrix Inst_World;
};

StructuredBuffer<SM_Instance> InstanceData : register(t25);


VertexOutput VS_Main(StaticMeshInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;

    
    float4 posWorld = mul(input.posObject, InstanceData[instanceID].Inst_World);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.ObjectNormal, (float3x3)InstanceData[instanceID].Inst_World));

    output.texCoord = input.texCoord;
    output.color = input.ObjectColor;


    float4 tangentWorld = float4(input.tangent, 0.0f);
    tangentWorld = mul(tangentWorld, InstanceData[instanceID].Inst_World);
    output.tangent = tangentWorld.xyz;


    return output;
}