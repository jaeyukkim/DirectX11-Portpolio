#include "Common.hlsli"




float4 PS_Main(VertexOutput input) : SV_TARGET
{

    return textureCube.Sample(g_sampler, input.posProj.xyz);

    //return float4(0.0f, 0.5f, 0.5f, 1.0f);
}