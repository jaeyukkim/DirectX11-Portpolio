#include "Common.hlsli"


struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

SamplingPixelShaderInput VS_Main(StaticMeshInput input)
{
    SamplingPixelShaderInput output;

    output.position = input.posObject;
    output.texcoord = input.texCoord;

    return output;
}
