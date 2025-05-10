#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Common.hlsli"

#define LIGHT_None 0
#define LIGHT_CubeMap 1
#define LIGHT_Directional 2
#define LIGHT_Spot 3
#define LIGHT_Point 4
#define LIGHT_Lim 5

struct Light
{
    int LightType;
    int LightID;
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
    float innerCone;
    float outerCone;
};


cbuffer CBLightCnt : register(b5)
{
    int LightCnt;
}

StructuredBuffer<Light> lights : register(t11);

/*
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
    float3 toEye, MaterialDesc mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 Specular = mat.Specular * pow(max(hdotn, 0.0f), 1.0f);

    return mat.Ambient + (mat.Diffuse + Specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, MaterialDesc mat, float3 normal,
    float3 toEye)
{
    float3 lightVec = -L.direction;

    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;

    // Luna DX12 å������ Specular ��꿡��
    // Lambert's law�� ����� lightStrength�� ����մϴ�.
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 ComputePointLight(Light L, MaterialDesc mat, float3 pos, float3 normal,
    float3 toEye)
{
    float3 lightVec = L.position - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= d;

        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;

        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}


float3 ComputeSpotLight(Light L, MaterialDesc mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;
    float d = length(lightVec);

    if (d > L.fallOffEnd)
        return float3(0.0f, 0.0f, 0.0f);

    lightVec = normalize(lightVec);

    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;

    float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
    lightStrength *= att;

   
    float spotCos = dot(L.direction, -lightVec);
    float rawSpotFactor = smoothstep(L.outerCone, L.innerCone, spotCos);
    float spotFactor = pow(rawSpotFactor, L.spotPower);

    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}
*/

float3 SchlickFresnel(float3 F0, float NdotH)
{
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
}


float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
    float metallic)
{
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance = textureCube[CUBEMAP_IRRADIENCE].Sample(g_sampler, normalWorld).rgb;

    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
    float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.Sample(ClampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness)).rg;
    float3 specularIrradiance = textureCube[CUBEMAP_SPECULAR].SampleLevel(g_sampler, reflect(-pixelToEye, normalWorld),
        3 + roughness * 5.0f).rgb;
    const float3 Fdielectric = 0.04; // ��ݼ�(Dielectric) ������ F0
    float3 F0 = lerp(Fdielectric, albedo, metallic);

    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
    float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);

    return (diffuseIBL + specularIBL) * (ao+3);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

    return alphaSq / (3.141592 * denom * denom);
}

// Single term for separable Schlick-GGX below.
float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float SchlickGGX(float NdotI, float NdotO, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

#endif // __LIGHTING_HLSLI__