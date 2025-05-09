#include "Common.hlsli"




float4 PS_Main(VertexOutput input) : SV_TARGET
{

   float3 viewDir = normalize(input.posWorld - EyePos);
   float3 ambient = textureCube[CUBEMAP_ENVTEX].Sample(g_sampler, viewDir);


   return float4(ambient, 1.0f);
}