#include "Common.hlsli"
#include "Sampler.hlsli"

Texture2D Tex : register(t0);

float4 PS_Main(VertexOutput Input) : SV_TARGET
{
	float3 color = Tex.SampleLevel(LinearWarpSampler, Input.texCoord, 0).rgb;
	return float4(color, 1.0f);
}