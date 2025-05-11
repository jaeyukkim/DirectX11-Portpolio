Texture2D BloomUpTex : register(t0);
SamplerState ClampSampler : register(s1);

cbuffer SamplingPixelConstantData : register(b6)
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

    float3 a = BloomUpTex.Sample(ClampSampler, float2(x - dx, y + dy)).rgb;
    float3 b = BloomUpTex.Sample(ClampSampler, float2(x, y + dy)).rgb;
    float3 c = BloomUpTex.Sample(ClampSampler, float2(x + dx, y + dy)).rgb;

    float3 d = BloomUpTex.Sample(ClampSampler, float2(x - dx, y)).rgb;
    float3 e = BloomUpTex.Sample(ClampSampler, float2(x, y)).rgb;
    float3 f = BloomUpTex.Sample(ClampSampler, float2(x + dx, y)).rgb;

    float3 g = BloomUpTex.Sample(ClampSampler, float2(x - dx, y - dy)).rgb;
    float3 h = BloomUpTex.Sample(ClampSampler, float2(x, y - dy)).rgb;
    float3 i = BloomUpTex.Sample(ClampSampler, float2(x + dx, y - dy)).rgb;

    float3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;

    return float4(color, 1.0);
    //return BloomUpTex.Sample(ClampSampler, input.texcoord);
}