#include "Common.hlsli"



VertexOutput VS_Main(VertexShaderInput input)
{
    VertexOutput output;
    float4 posWorld;
    
    posWorld = mul(float4(input.posModel, 1.0f), World);
    output.posProj = mul(posWorld, ViewProjection);
    output.texCoord = input.texCoord;
   

    return output;
}


