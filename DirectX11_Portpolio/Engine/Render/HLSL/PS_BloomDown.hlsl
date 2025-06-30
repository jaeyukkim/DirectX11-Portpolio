#include "Sampler.hlsli"

Texture2D BloomDownTex : register(t0);


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
    float x = input.texcoord.x;
    float y = input.texcoord.y;

    float3 a = BloomDownTex.Sample(LinearClampSampler, float2(x - 2 * dx, y + 2 * dy)).rgb;
    float3 b = BloomDownTex.Sample(LinearClampSampler, float2(x, y + 2 * dy)).rgb;
    float3 c = BloomDownTex.Sample(LinearClampSampler, float2(x + 2 * dx, y + 2 * dy)).rgb;

    float3 d = BloomDownTex.Sample(LinearClampSampler, float2(x - 2 * dx, y)).rgb;
    float3 e = BloomDownTex.Sample(LinearClampSampler, float2(x, y)).rgb;
    float3 f = BloomDownTex.Sample(LinearClampSampler, float2(x + 2 * dx, y)).rgb;

    float3 g = BloomDownTex.Sample(LinearClampSampler, float2(x - 2 * dx, y - 2 * dy)).rgb;
    float3 h = BloomDownTex.Sample(LinearClampSampler, float2(x, y - 2 * dy)).rgb;
    float3 i = BloomDownTex.Sample(LinearClampSampler, float2(x + 2 * dx, y - 2 * dy)).rgb;

    float3 j = BloomDownTex.Sample(LinearClampSampler, float2(x - dx, y + dy)).rgb;
    float3 k = BloomDownTex.Sample(LinearClampSampler, float2(x + dx, y + dy)).rgb;
    float3 l = BloomDownTex.Sample(LinearClampSampler, float2(x - dx, y - dy)).rgb;
    float3 m = BloomDownTex.Sample(LinearClampSampler, float2(x + dx, y - dy)).rgb;

    float3 color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.125;

    return float4(color, 1.0);
    //return BloomDownTex.Sample(ClampSampler, input.texcoord);
}