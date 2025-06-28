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
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 modelNormal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR; // Normal lines 쉐이더에서 사용
    float3 tangent : TANGENT;
};

struct StaticMeshInput
{
    float4 posObject : POSITION; // 오브젝트의 정점 position
    float2 texCoord : TEXCOORD; // 텍스쳐 uv 좌표
    float4 ObjectColor : COLOR;
    float3 ObjectNormal : NORMAL; // 오브젝트의 정점의 normal    
    float3 tangent : TANGENT;
};

struct VertexShaderInput
{
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float2 texCoord : TEXCOORD; // <- 다음 예제에서 사용
    float4 modelColor : COLOR;
    float3 modelNormal : NORMAL; // 모델 좌표계의 normal    
    float3 tangent : TANGENT;
    float4 blendIndicies : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHTS;
};

int RaySphereIntersection(in float3 start, in float3 dir, in float3 center, in float radius,
                            out float t1, out float t2)
{
    float3 p = start - center;
    float pdotv = dot(p, dir);
    float p2 = dot(p, p);
    float r2 = radius * radius;
    float m = pdotv * pdotv - (p2 - r2);
    
    if (m < 0.0)
    {
        t1 = 0;
        t2 = 0;
        return 0;
    }
    else
    {
        m = sqrt(m);
        t1 = -pdotv - m;
        t2 = -pdotv + m;
        return 1;
    }
}

#endif // __COMMON_HLSLI__