#include "Lighting.hlsli"


float4 PS_Main(VertexOutput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}