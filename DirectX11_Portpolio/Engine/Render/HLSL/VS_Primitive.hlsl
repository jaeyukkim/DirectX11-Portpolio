#include "Common.hlsli"

VertexOutput VS_Main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;


    float4 posWorld = mul(float4(input.posModel, 1.0f), InstanceDatas[instanceID].Inst_World);
    output.posProj = mul(posWorld, ViewProjection);
    output.color = float3(0.0f, 1.0f, 0.0f);
    
    return output;
}