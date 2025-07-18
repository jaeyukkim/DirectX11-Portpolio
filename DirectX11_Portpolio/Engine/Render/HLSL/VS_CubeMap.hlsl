#include "Common.hlsli"

struct SkyboxPSInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};


SkyboxPSInput VS_Main(VertexShaderInput input)
{

    SkyboxPSInput output;


    
    output.posProj = mul(float4(input.posModel.xyz, 0.0f), World);
    output.posModel = output.posProj.xyz;
    output.posProj = mul(float4(output.posProj.xyz, 0.0f), View);
    output.posProj = mul(float4(output.posProj.xyz, 1.0f), Projection);

    return output;
}
