#ifndef __MATERIAL_HLSLI__
#define __MATERIAL_HLSLI__

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
#define MAX_CUBEMAP_TEXTURE_COUNT 3 // BRDF는 2D 텍스쳐임


TextureCube textureCube[MAX_CUBEMAP_TEXTURE_COUNT] : register(t0);
Texture2D brdfTex  : register(t3);
Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT]  : register(t4);

SamplerState g_sampler : register(s0);
SamplerState ClampSampler : register(s1);

static const float3 Fdielectric = 0.04;  // 비금속(Dielectric) 재질의 F0


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

float3 ApplyNormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float3 normalWorld = normal;

    if (Material.useNormalMap)
    {
        float3 NewNormal = MaterialMaps[TEXTURE_NORMAL].SampleLevel(samp, uv, 0.0).rgb;
        NewNormal = 2.0 * NewNormal - 1.0;

        // OpenGL 노멀맵일 경우에는 y 방향을 뒤집어줍니다.
        NewNormal.y = Material.invertNormalMapY ? -NewNormal.y : NewNormal.y;


        float3 N = normal;
        float3 T = normalize(tangent - dot(tangent, N) * N);
        float3 B = cross(N, T);

        float3x3 TBN = float3x3(T, B, N);
        normalWorld =  normalize(mul(NewNormal, TBN));   // 변환된 노멀 반환
    }

    return normalWorld;
}


#endif  //__MATERIAL_HLSLI__