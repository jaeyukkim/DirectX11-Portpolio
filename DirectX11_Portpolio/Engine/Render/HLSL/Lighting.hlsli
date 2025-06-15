#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Common.hlsli"

#define LIGHT_None (1 << 0)
#define LIGHT_Directional (1 << 1)
#define LIGHT_Spot (1 << 2)
#define LIGHT_Point (1 << 3)
#define LIGHT_Lim (1 << 4)

struct Light
{
    int Type;
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


cbuffer CBLightInfo : register(b5)
{
    int LightCnt;
    float IBLStrength;
}

StructuredBuffer<Light> lights : register(t11);


float3 ComputeLimLight(Light light, float3 toEye, float3 finalNormal)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float rim = (1.0f - dot(finalNormal, toEye));
    rim = smoothstep(0.0f, 1.0f, rim);
    rim = pow(abs(rim), light.spotPower);
    color += rim * color * length(light.strength);

    return color;
}

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
    float3 irradiance = textureCube[CUBEMAP_IRRADIENCE].SampleLevel(LinearWarpSampler, normalWorld, 0).rgb;

    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
    float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.SampleLevel(LinearClampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness), 0.0f).rg;
    float3 specularIrradiance = textureCube[CUBEMAP_SPECULAR].SampleLevel(LinearWarpSampler, reflect(-pixelToEye, normalWorld),
        2 + roughness * 5.0f).rgb;
    float3 F0 = lerp(Fdielectric, albedo, metallic);

    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
    float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);

    return (diffuseIBL + specularIBL) * ao;
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


float3 LightRadiance(Light light, float3 posWorld, float3 normalWorld)
{

    // Directional light
    float3 lightVec = light.Type & LIGHT_Directional
        ? -light.direction
        : light.position - posWorld;

    float lightDist = length(lightVec);
    lightVec /= lightDist;

    // Spot light
    float spotFactor = 1.0f;
    if (light.Type & LIGHT_Spot)
    {
        float cosTheta = max(-dot(lightVec, light.direction), 0.0f);
        float raw = smoothstep(light.outerCone, light.innerCone, cosTheta); // inner > outer!
        spotFactor = pow(raw, light.spotPower);
    }


    // Distance attenuation
    float att = saturate((light.fallOffEnd - lightDist)
        / (light.fallOffEnd - light.fallOffStart));

    // Shadow map
    float shadowFactor = 1.0;

    float3 radiance = light.strength * spotFactor * att * shadowFactor;

    return radiance;
}

#endif // __LIGHTING_HLSLI__