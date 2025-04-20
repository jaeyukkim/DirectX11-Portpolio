#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Common.hlsli"

#define LIGHT_CubeMap 0
#define LIGHT_Directional 1
#define LIGHT_Spot 2
#define LIGHT_Point 3


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
    float4 color;
};

cbuffer CBLightCnt : register(b6)
{
    int LightCnt;
}

StructuredBuffer<Light> lights : register(t5);


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

    // Luna DX12 책에서는 Specular 계산에도
    // Lambert's law가 적용된 lightStrength를 사용합니다.
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

    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);

    // 너무 멀면 조명이 적용되지 않음
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

float3 ComputeSpotLight(Light L, MaterialDesc mat, float3 pos, float3 normal,
    float3 toEye)
{
    float3 lightVec = L.position - pos;

    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);

    // 너무 멀면 조명이 적용되지 않음
    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= d;

        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;

        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        float spotFactor = pow(max(-dot(lightVec, L.direction), 0.0f), L.spotPower);
        lightStrength *= spotFactor;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }

}



#endif // __LIGHTING_HLSLI__