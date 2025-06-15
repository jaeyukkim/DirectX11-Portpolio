#include "Common.hlsli"



VertexOutput VS_Main(StaticMeshInput input)
{

    VertexOutput output;


    float4 posWorld = mul(input.posObject, World);
    output.posWorld = posWorld.xyz;
    output.posProj = mul(posWorld, ViewProjection);

    return output;
}
