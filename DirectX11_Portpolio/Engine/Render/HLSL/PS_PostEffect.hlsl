#include "Lighting.hlsli" 


Texture2D renderTex : register(t20); // Rendering results
Texture2D depthOnlyTex : register(t21); // DepthOnly


// 1: Rendered image, 2: DepthOnly
cbuffer PostEffectsConstants : register(b8)
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
	posProj.y *= -1; // y ÁÂÇ¥ µÚÁý±â
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
		float3 color = renderTex.Sample(LinearClampSampler, input.texcoord).rgb;
		float4 posView = TexcoordToView(input.texcoord);


		
		// Halo
		float3 haloColor = float3(0.96, 0.94, 0.82);

		
		[unroll]
		for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
		{
			if(Lights[i].Type & LIGHT_Halo)
			{
				float radius = Lights[i].radius;
				float L_strength = length(Lights[i].strength);
				
				color += HaloEmission(Lights[i], posView.xyz, radius) * haloColor * L_strength;
			}
		}
		

		// Fog
		float dist = length(posView.xyz); // ´«ÀÇ À§Ä¡°¡ ¿øÁ¡ÀÎ ÁÂÇ¥°è
		float3 fogColor = float3(1, 1, 1);
		float fogMin = 1.0;
		float fogMax = 10.0;
		float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
		float fogFactor = exp(-distFog * fogStrength);

		color = lerp(fogColor, color, fogFactor);
        
		return float4(color, 1.0);
	}
	else // if (mode == 2)
	{
		float z = TexcoordToView(input.texcoord).z * depthScale;
		return float4(z, z, z, 1);
	}
	
}
