#include "Sampler.hlsli"
Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);


cbuffer ImageFilterConstData : register(b7)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float exposure;
    float gamma;
    float option3;
    float option4;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float3 FilmicToneMapping(float3 color)
{
    color = max(float3(0, 0, 0), color);
    color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / gamma;

    color = clamp(exposure * color, 0., 1.);
    color = pow(color, invGamma);
    return color;
}

float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;

    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, float3(1.0, 1.0, 1.0) / gamma);
    return color;
}

float3 lumaBasedReinhardToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / gamma;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, invGamma);
    return color;
}

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    // Pre-exposure (optional)
    color *= 0.6f;

    // Apply ACES curve
    color = RRTAndODTFit(color);

    // Gamma correction (convert to sRGB)
    color = pow(color, 1.0 / 2.2); // or use sRGB conversion

    return color;
}

float4 PS_Main(SamplingPixelShaderInput input) : SV_TARGET
{
    float3 color0 = g_texture0.Sample(LinearClampSampler, input.texcoord).rgb;
    float3 color1 = g_texture1.Sample(LinearClampSampler, input.texcoord).rgb;

    // Bloom 텍스처의 밝기 계산 (luminance)
    float brightness = dot(color1, float3(0.2126, 0.7152, 0.0722)); // sRGB 기준 국제 색상 표준 가중 휘도 계수

    // threshold 이하이면 bloom 영향 제거
    float bloomFactor = (brightness > threshold) ? strength : 0.0;

    // 선형 보간 방식으로 적용
    float3 combined = lerp(color0, color1, bloomFactor);

    // 톤매핑
    combined = ACESFitted(combined);
    return float4(combined, 1.0f);

    
    /*
    float3 color0 = g_texture0.Sample(LinearClampSampler, input.texcoord).rgb;
    float3 color1 = g_texture1.Sample(LinearClampSampler, input.texcoord).rgb;

    float3 combined = (1.0 - strength) * color0 + strength * color1;

    // Tone Mapping  
    combined = ACESFitted(combined);

    return float4(combined, 1.0f);
    */

    /*
    float3 color0 = g_texture0.Sample(LinearClampSampler, input.texcoord).rgb;
    float3 color1 = g_texture1.Sample(LinearClampSampler, input.texcoord).rgb;

    float l = (color1.x + color1.y + color1.y) / 3;
    
    float3 combined = l>threshold ? (1.0 - strength) * color0 + strength * color1
    : color0;

    // Tone Mapping  
    combined = ACESFitted(combined);

    return float4(combined, 1.0f);
     */
}
