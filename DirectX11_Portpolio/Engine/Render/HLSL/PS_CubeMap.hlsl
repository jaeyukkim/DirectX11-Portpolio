#include "Lighting.hlsli"


struct SkyboxPSInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};


float4 PS_Main(SkyboxPSInput input) : SV_TARGET
{

   float3 ambient = textureCube[CUBEMAP_ENVTEX].SampleLevel(LinearWarpSampler, input.posModel.xyz, 0);

   ambient *= IBLStrength;
   return float4(ambient, 1.0f);
}
