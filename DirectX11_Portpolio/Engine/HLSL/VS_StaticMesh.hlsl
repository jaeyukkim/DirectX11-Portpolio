#include "Common.hlsli"

struct StaticMeshInput
{
    float4 posObject : POSITION; // 오브젝트의 정점 position
    float2 texCoord : TEXCOORD; // 텍스쳐 uv 좌표
    float4 ObjectColor : COLOR;
    float3 ObjectNormal : NORMAL; // 오브젝트의 정점의 normal    
    float3 tanzent : TANGENT;
};

VertexOutput VS_Main(StaticMeshInput input)
{
    VertexOutput output;


    float4 posWorld = mul(input.posObject, World);
    output.posProj = mul(posWorld, ViewProjection);

    output.posWorld = posWorld.xyz;

    output.modelNormal = normalize(mul(input.ObjectNormal, (float3x3)World));
    output.texCoord = input.texCoord;
    output.color = input.ObjectColor;

    return output;
}