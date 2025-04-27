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
    float3 color = input.color;
    float2 uv = input.texCoord * Material.UV_Tiling + Material.UV_Offset;
    float3 finalNormal = ApplyNormalMapping(uv, input.modelNormal, input.tangent, g_sampler);

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
            color += ComputeDirectionalLight(L, Material, finalNormal, toEye);
            break;

        case LIGHT_Point:
            color += ComputePointLight(L, Material, input.posWorld, finalNormal, toEye);
            break;

        case LIGHT_Spot:
            color += ComputeSpotLight(L, Material, input.posWorld, finalNormal, toEye);
            break;

        case LIGHT_Lim:
            float rim = (1.0f - dot(finalNormal, toEye));
            rim = smoothstep(0.0f, 1.0f, rim);
            rim = pow(abs(rim), L.spotPower);
            color += rim * color * length(L.strength);
            break;
        }
    }


    return float4(color, 1.0f) * MaterialMaps[MATERIAL_TEXTURE_Diffuse].Sample(g_sampler, uv);


    
}

