#include "Common.hlsli"

struct StaticMeshInput
{
    float4 posObject : POSITION; // ������Ʈ�� ���� position
    float2 texCoord : TEXCOORD; // �ؽ��� uv ��ǥ
    float4 ObjectColor : COLOR;
    float3 ObjectNormal : NORMAL; // ������Ʈ�� ������ normal    
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