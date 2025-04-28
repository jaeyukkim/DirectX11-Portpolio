#ifndef __MATERIAL_HLSLI__
#define __MATERIAL_HLSLI__


#define MATERIAL_TEXTURE_Diffuse 0
#define MATERIAL_TEXTURE_Specular 1
#define MATERIAL_TEXTURE_NORMAL 2
#define MAX_MATERIAL_TEXTURE_COUNT 3

TextureCube textureCube : register(t0);
Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT]  : register(t6);

SamplerState g_sampler : register(s0);

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    float2 UV_Tiling;
    float2 UV_Offset;
};

cbuffer CB_Material : register(b0)
{
    MaterialDesc Material;
};


float3 ApplyNormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float4 map = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(samp, uv);

    [flatten]
    if (any(map.rgb) == false)
        return normal;   // 노말맵 없으면 원본 노멀 반환

    float3 coord = map.rgb * 2.0f - 1.0f;

    float3 N = normalize(normal);
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);
    return normalize(mul(coord, TBN));   // 변환된 노멀 반환
}


#endif // __MATERIAL_HLSLI__