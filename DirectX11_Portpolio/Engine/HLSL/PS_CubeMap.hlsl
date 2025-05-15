#include "Lighting.hlsli"


float4 PS_Main(VertexOutput input) : SV_TARGET
{

   float3 viewDir = normalize(input.posWorld - EyePos);
   float3 ambient = textureCube[CUBEMAP_ENVTEX].SampleLevel(g_sampler, viewDir, 0);

   ambient *= IBLStrength;
   return float4(ambient, 1.0f);
}
