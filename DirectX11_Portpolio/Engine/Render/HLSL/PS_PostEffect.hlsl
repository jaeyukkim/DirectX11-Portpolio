#include "Common.hlsli" 


Texture2D renderTex : register(t20); // Rendering results
Texture2D depthOnlyTex : register(t21); // DepthOnly


// 1: Rendered image, 2: DepthOnly
cbuffer PostEffectsConstants : register(b7)
{
	int mode; 
	float depthScale;    
	float fogStrength;
};


struct SamplingPixelShaderInput
{
	float4 posProj : SV_POSITION;
	float2 texcoord : TEXCOORD;
};


float4 TexcoordToView(float2 texcoord)
{
	float4 posProj;

	// [0, 1]x[0, 1] -> [-1, 1]x[-1, 1]
	posProj.xy = texcoord * 2.0 - 1.0;
	posProj.y *= -1; // y ��ǥ ������
	posProj.z = depthOnlyTex.Sample(LinearClampSampler, texcoord).r;
	posProj.w = 1.0;
    
	// ProjectSpace -> ViewSpace
	float4 posView = mul(posProj, ProjectionInverse);
	posView.xyz /= posView.w;
    
	return posView;
}

float4 PS_Main(SamplingPixelShaderInput input) : SV_TARGET
{
	if (mode == 1)
	{
		float4 posView = TexcoordToView(input.texcoord);
		float dist = length(posView.xyz); // ���� ��ġ�� ������ ��ǥ��

		// Beer-Lambert Law
		// fogStrength : extinction coefficient
        
		// Fog
		float3 fogColor = float3(1, 1, 1);
		float fogMin = 1.0;
		float fogMax = 10.0;
        
		float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
		float fogFactor = exp(-distFog * fogStrength);

		float3 color = renderTex.Sample(LinearClampSampler, input.texcoord).rgb;
        
		color = lerp(fogColor, color, fogFactor);
		return float4(color, 1.0);
	}
	else // if (mode == 2)
	{
		float z = TexcoordToView(input.texcoord).z * depthScale;
		return float4(z, z, z, 1);
	}
}
