#include "Common.hlsli"
#include "Sampler.hlsli"

Texture2D Tex : register(t0);

float4 PS_Main(VertexOutput Input) : SV_TARGET
{
	return Tex.Sample(LinearWarpSampler, Input.texCoord);
}