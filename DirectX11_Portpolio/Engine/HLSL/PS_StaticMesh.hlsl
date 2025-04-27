#include "Lighting.hlsli"


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
            // ���� �̱���
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