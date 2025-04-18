#include "Common.hlsli"


/*
Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light light[MAX_LIGHTS];
};*/

float4 PS_Main(VertexOutput input) : SV_TARGET
{
    // float3 normal = normalize(input.Normal);
    //float light = dot(-LightDirection, normal);

    //float3 color = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(Samp, input.Uv).rgb;
    //color *= light;

    return float4(input.color, 1.0f);
}