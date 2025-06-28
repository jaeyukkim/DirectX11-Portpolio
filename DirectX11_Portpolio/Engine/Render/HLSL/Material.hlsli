#ifndef __MATERIAL_HLSLI__
#define __MATERIAL_HLSLI__

#include "Sampler.hlsli"

#define TEXTURE_ALBEDO 0
#define TEXTURE_METALLIC 1
#define TEXTURE_ROUGHNESS 2
#define TEXTURE_NORMAL 3
#define TEXTURE_AMBIENTOCCLUSION 4
#define TEXTURE_EMISSIVE 5
#define TEXTURE_HEIGHT 6
#define MAX_MATERIAL_TEXTURE_COUNT 7

#define CUBEMAP_ENVTEX 0
#define CUBEMAP_SPECULAR 1
#define CUBEMAP_IRRADIENCE 2
#define CUBEMAP_BRDF 3
#define MAX_CUBEMAP_TEXTURE_COUNT 3 // BRDF�� 2D �ؽ�����

#define MaxLOD 10
#define BasicLodScale 10
#define NormalLod 4


TextureCube textureCube[MAX_CUBEMAP_TEXTURE_COUNT] : register(t0);
Texture2D brdfTex  : register(t3);
Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT]  : register(t4);



static const float3 Fdielectric = 0.04;  // ��ݼ�(Dielectric) ������ F0


struct MaterialDesc
{
    float4 Albedo;
    float Roughness;
    float Metallic;
    float2 padding;
    float4 Emissive;
    float2 UV_Tiling;
    float2 UV_Offset;
    int useAlbedoMap;
    int useNormalMap;
    int useAOMap;
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    int padding2;
};

cbuffer CB_Material : register(b0)
{
    MaterialDesc Material;
};

float ComputeLODBasedOnLog2(float distance, float lodScale)
{
    float lod = log2((distance + 1.0f) / lodScale);
    return clamp(lod, 0, MaxLOD);
}

float ComputeLODBasedOnLog3(float3 toEye, float3 normal, float distance, float lodScale)
{
    
    float lod = log2((distance + 1.0f) / lodScale);

    float cosAngle = saturate(dot(normal, toEye));
    float angleFactor = 1.0f - cosAngle;

    // ������ ������ ���� Ŀ�� ����. 
    // angleFactor�� 0.293 (cos 45�� ����)�� �� �� +1 �ǵ��� ����
    float lodOffset = 0.0f;

    if (angleFactor <= 0.2f)
    {
        // ���� ����: 0 ~ 0.293 ���� �� LOD +0 ~ +1
        lodOffset = angleFactor / 0.2f;
    }
    else
    {
        // �� ���ĺ��ʹ� ������ ����: Ŀ�긦 ���ϰ� ����
        float x = (angleFactor - 0.2f) / (1.0f - 0.2f); // [0, 1] ����ȭ
        float expGrowth = pow(x, 3.0f);                     // ���� ���� (2.0~3.0 ���� ����)
        lodOffset = 1.0f + expGrowth * 7.0f;                // 1~5 �������� ���
    }

    return clamp(lod + lodOffset, 0.0f, MaxLOD);
    
}


float3 ApplyNormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp, float LOD)
{
    float3 normalWorld = normal;

    if (Material.useNormalMap)
    {
        float3 NewNormal = MaterialMaps[TEXTURE_NORMAL].SampleLevel(samp, uv, LOD).rgb;
        NewNormal = 2.0 * NewNormal - 1.0;

        // OpenGL ��ָ��� ��쿡�� y ������ �������ݴϴ�.
        NewNormal.y = Material.invertNormalMapY ? -NewNormal.y : NewNormal.y;


        float3 N = normal;
        float3 T = normalize(tangent - dot(tangent, N) * N);
        float3 B = cross(N, T);

        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(NewNormal, TBN));   // ��ȯ�� ��� ��ȯ
    }

    return normalWorld;
}


#endif  //__MATERIAL_HLSLI__