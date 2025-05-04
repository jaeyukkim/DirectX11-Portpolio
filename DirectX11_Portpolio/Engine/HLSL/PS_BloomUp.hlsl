Texture2D BloomDownTex : register(t0);
SamplerState g_sampler : register(s0);

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
    return BloomDownTex.Sample(g_sampler, input.texcoord);
}