#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#include "Material.hlsli"



cbuffer CB_World : register(b1)
{
    matrix World;
};

cbuffer CB_ViewContext : register(b2)
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix ProjectionInverse;
    matrix ViewProjection;
    float3 EyePos;
}


struct VertexOutput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (���� ��꿡 ���)
    float3 modelNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR; // Normal lines ���̴����� ���
    float3 tangent : TANGENT;
};

struct StaticMeshInput
{
    float4 posObject : POSITION; // ������Ʈ�� ���� position
    float2 texCoord : TEXCOORD; // �ؽ��� uv ��ǥ
    float4 ObjectColor : COLOR;
    float3 ObjectNormal : NORMAL; // ������Ʈ�� ������ normal    
    float3 tangent : TANGENT;
};

struct VertexShaderInput
{
    float3 posModel : POSITION; //�� ��ǥ���� ��ġ position
    float2 texCoord : TEXCOORD; // <- ���� �������� ���
    float4 modelColor : COLOR;
    float3 modelNormal : NORMAL; // �� ��ǥ���� normal    
    float3 tangent : TANGENT;
    float4 blendIndicies : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHTS;
};
#endif // __COMMON_HLSLI__