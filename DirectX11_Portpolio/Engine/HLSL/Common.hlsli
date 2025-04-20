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
};

#endif // __COMMON_HLSLI__