#include "Sampler.hlsli"

Texture2D g_texture0 : register(t0);


cbuffer SamplingPixelConstantData : register(b7)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float4 options;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 PS_Main(SamplingPixelShaderInput input) : SV_TARGET
{
    float3 color = g_texture0.Sample(LinearWarpSampler, input.texcoord).rgb;
    float l = (color.x + color.y + color.y) / 3;

    if (l > threshold)
    {
        return float4(color, 1.0f);
    }
    else
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}