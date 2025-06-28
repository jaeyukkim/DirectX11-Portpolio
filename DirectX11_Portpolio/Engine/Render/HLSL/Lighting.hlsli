#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Common.hlsli"
#define MAX_LIGHT_COUNT 9
#define MAX_SHADOW_COUNT 9

#define NEAR_PLANE 1.0
#define LIGHT_FRUSTUM_WIDTH 1.53465 // <- 계산해서 찾은 값


#define LIGHT_None 0
#define LIGHT_Directional (1 << 0)
#define LIGHT_Spot (1 << 1)
#define LIGHT_Point (1 << 2)
#define LIGHT_Lim (1 << 3)
#define LIGHT_Halo (1 << 4)
#define Use_Shadow (1 << 5)

Texture2D ShadowMap[MAX_SHADOW_COUNT] : register(t11);

struct Light
{
    matrix viewProj;
    matrix invProj;

    float3 strength;
    float Lpadding0;
    float3 direction;
    float Lpadding1;
    float3 position;
    float Lpadding2;

    int Type;
    int LightID;
    float radius;
    float spotPower;

    float fallOffStart;
    float fallOffEnd;
    float innerCone;
    float outerCone;

};


cbuffer CBLightObject : register(b5)
{
    Light Lights[MAX_LIGHT_COUNT];
}


cbuffer CBLightInfo : register(b6)
{
    int LightCnt;
    float IBLStrength;
    int ShadowCount;
    float Light_padding;
}


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


// 참고: https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/ShadowMapping.fragmentshader
float random(float3 seed, int i)
{
    float4 seed4 = float4(seed, i);
    float dot_product = dot(seed4, float4(12.9898, 78.233, 45.164, 94.673));
    return frac(sin(dot_product) * 43758.5453);
}

// NdcDepthToViewDepth
float N2V(float ndcDepth, matrix invProj)
{
    float4 pointView = mul(float4(0, 0, ndcDepth, 1), invProj);
    return pointView.z / pointView.w;
}


float PCF_Filter(float2 uv, float zReceiverNdc, float filterRadiusUV, Texture2D shadowMap)
{
    float sum = 0.0f;
    for (int i = 0; i < 64; ++i)
    {
        float2 offset = diskSamples64[i] * filterRadiusUV;
        sum += shadowMap.SampleCmpLevelZero(
            shadowCompareSampler, uv + offset, zReceiverNdc);
    }
    return sum / 64;
}


void FindBlocker(out float avgBlockerDepthView, out float numBlockers, float2 uv,
    float zReceiverView, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;

    float searchRadius = lightRadiusUV * (zReceiverView - NEAR_PLANE) / zReceiverView;

    float blockerSum = 0;
    numBlockers = 0;
    for (int i = 0; i < 64; ++i)
    {
        float shadowMapDepth =
            shadowMap.SampleLevel(shadowPointSampler, float2(uv + diskSamples64[i] * searchRadius), 0).r;

        shadowMapDepth = N2V(shadowMapDepth, invProj);

        if (shadowMapDepth < zReceiverView)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
    avgBlockerDepthView = blockerSum / numBlockers;
}

float PCSS(float2 uv, float zReceiverNdc, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;

    float zReceiverView = N2V(zReceiverNdc, invProj);

    // STEP 1: blocker search
    float avgBlockerDepthView = 0;
    float numBlockers = 0;

    FindBlocker(avgBlockerDepthView, numBlockers, uv, zReceiverView, shadowMap, invProj, lightRadiusWorld);

    if (numBlockers < 1)
    {
        // There are no occluders so early out(this saves filtering)
        return 1.0f;
    }
    else
    {
        // STEP 2: penumbra size
        float penumbraRatio = (zReceiverView - avgBlockerDepthView) / avgBlockerDepthView;
        float filterRadiusUV = penumbraRatio * lightRadiusUV * NEAR_PLANE / zReceiverView;

        // STEP 3: filtering
        return PCF_Filter(uv, zReceiverNdc, filterRadiusUV, shadowMap);
    }
}


float3 LightRadiance(Light light, float3 posWorld, float3 normalWorld, Texture2D shadowMap)
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
    
    if (light.Type & Use_Shadow)
    {
        const float nearZ = 0.01; // 카메라 설정과 동일

        // 1. Project posWorld to light screen    
        float4 lightScreen = mul(float4(posWorld, 1.0), light.viewProj);
        lightScreen.xyz /= lightScreen.w;

        // 2. 카메라(광원)에서 볼 때의 텍스춰 좌표 계산
        float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
        lightTexcoord += 1.0;
        lightTexcoord *= 0.5;

        // 3. 쉐도우맵에서 값 가져오기
        //float depth = shadowMap.Sample(shadowPointSampler, lightTexcoord).r;

        // 4. 가려져 있다면 그림자로 표시
        //if (depth + 0.001 < lightScreen.z)
          //  shadowFactor = 0.0;

        uint width, height, numMips;
        shadowMap.GetDimensions(0, width, height, numMips);

        // Texel size
        float dx = 5.0 / (float)width;
        // shadowFactor = PCF_Filter(lightTexcoord.xy, lightScreen.z - 0.001, dx, shadowMap);
        shadowFactor = PCSS(lightTexcoord, lightScreen.z - 0.01, shadowMap, light.invProj, light.radius);
    }
    
    float3 radiance = light.strength * spotFactor * att * shadowFactor;

    return radiance;
}



float HaloEmission(Light light, float3 posView, float radius)
{
    // Halo
    float3 rayStart = float3(0, 0, 0); // View space
    float3 dir = normalize(posView - rayStart);

    float3 center = mul(float4(light.position, 1.0), View).xyz; // View 공간으로 변환

    float t1 = 0.0;
    float t2 = 0.0;
    
    if (RaySphereIntersection(rayStart, dir, center, radius, t1, t2) && t1 < posView.z)
    {
        t2 = min(posView.z, t2);
        
        float3 p = rayStart - center;
        float p2 = dot(p, p);
        float R2 = radius*radius;
        float invR2 = 1.0f / R2;

 
        float haloEmission =  (1-p2 * invR2) * (t2-t1)
                                -dot(p, dir) * invR2 * (t2*t2 - t1*t1)
                                -(1.0f / (3.0f * R2)) * (t2*t2*t2 - t1*t1*t1);

        haloEmission /= (4*radius/3.0);
        return haloEmission;
    }
    else
    {
        return 0.0;
    }
}

#endif // __LIGHTING_HLSLI__