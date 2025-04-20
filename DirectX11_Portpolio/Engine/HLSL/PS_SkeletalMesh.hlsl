#include "Lighting.hlsli"


/*
Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light light[MAX_LIGHTS];
};*/

/*
* float3 normal = normalize(input.modelNormal);
    float light = dot(-float3(-1, -1, 1), normal);

    float3 color = MaterialMaps[MATERIAL_TEXTURE_Diffuse].Sample(g_sampler, input.texCoord).rgb;
    color *= light;

   return float4(color,1);
 */
float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float3 toEye = normalize(EyePos - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);
    
    int i = 0;

  
    for (int i = 0; i < LightCnt; ++i)
    {
        Light L = lights[i];

        switch (L.LightType)
        {
        case LIGHT_CubeMap:
            // 아직 미구현
                break;

        case LIGHT_Directional:
            color += ComputeDirectionalLight(L, Material, input.modelNormal, toEye);
            break;

        case LIGHT_Point:
            color += ComputePointLight(L, Material, input.posWorld, input.modelNormal, toEye);
            break;

        case LIGHT_Spot:
            color += ComputeSpotLight(L, Material, input.posWorld, input.modelNormal, toEye);
            break;
        }
    }

    return float4(color, 1.0f) * MaterialMaps[MATERIAL_TEXTURE_Diffuse].Sample(g_sampler, input.texCoord);
    
    
}

