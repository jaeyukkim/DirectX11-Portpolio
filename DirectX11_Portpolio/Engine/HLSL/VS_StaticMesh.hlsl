#include "Common.hlsli"



VertexOutput VS_Main(StaticMeshInput input)
{
    VertexOutput output;


    float4 posWorld = mul(input.posObject, World);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.ObjectNormal, (float3x3)World));

    output.texCoord = input.texCoord;
    output.color = input.ObjectColor;

    
    float4 tangentWorld = float4(input.tangent, 0.0f);
    tangentWorld = mul(tangentWorld, World);
    output.tangent = tangentWorld.xyz;
 

    return output;
}