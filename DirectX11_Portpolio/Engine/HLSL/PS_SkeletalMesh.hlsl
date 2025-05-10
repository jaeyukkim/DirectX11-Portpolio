#include "Lighting.hlsli"



float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float4 PixelColor = float4(input.color, 1.0f);
    
    float3 toEye = normalize(EyePos - input.posWorld);
    float2 uv = input.texCoord * Material.UV_Tiling + Material.UV_Offset;
    float3 finalNormal = ApplyNormalMapping(uv, input.modelNormal, input.tangent, g_sampler);
    
    
    float3 albedo = Material.useAlbedoMap ? MaterialMaps[TEXTURE_ALBEDO].Sample(g_sampler, uv).rgb : Material.Albedo;
    float ao = Material.useAOMap ? MaterialMaps[TEXTURE_AMBIENTOCCLUSION].SampleLevel(g_sampler, uv, 0.0).r : 1.0f;
   // float metallic = Material.useMetallicMap ? MaterialMaps[TEXTURE_METALLIC].Sample(g_sampler, uv).r
   //     : Material.Metallic;
    float metallic = MaterialMaps[TEXTURE_METALLIC].Sample(g_sampler, uv).r;
    float roughness = Material.useRoughnessMap ? MaterialMaps[TEXTURE_ROUGHNESS].Sample(g_sampler, uv).r
        : Material.Roughness;
    float3 emission = Material.useEmissiveMap ? MaterialMaps[TEXTURE_EMISSIVE].Sample(g_sampler, uv).rgb
        : float3(0, 0, 0);
    
    float3 ambientLighting = AmbientLightingByIBL(albedo, finalNormal, toEye, ao, metallic, roughness);

    //color = float4(ambientLighting + directLighting + emission, 1.0);
     //PixelColor = float4(ambientLighting + emission, 1.0);
    PixelColor = float4(metallic, metallic, metallic, 1.0);
    return PixelColor;
       // clamp(PixelColor, 0.0, 1000.0);
    
/*
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


    float3 R = normalize(reflect(-toEye, finalNormal));

    float3 iblDiffuse = textureCube[CUBEMAP_SPECULAR].Sample(g_sampler, finalNormal);
    float3 iblSpecular = textureCube[CUBEMAP_IRRADIENCE].Sample(g_sampler, R);

    float shininess = Material.Shininess;
    iblSpecular *= pow(saturate(dot(finalNormal, toEye)), shininess);

    
    float specStrength = MaterialMaps[TEXTURE_DIFFUSE_ROUGHNESS].Sample(g_sampler, uv).r;

    iblSpecular *= specStrength;
    iblDiffuse *= Material.Diffuse.rgb;
    iblSpecular *= Material.Specular.rgb;

    float3 baseDiffuse = MaterialMaps[TEXTURE_METALLIC].Sample(g_sampler, uv).rgb;
    float3 finalColor = baseDiffuse * (color + iblDiffuse) + iblSpecular;

    return float4(saturate(finalColor), 1.0f);
    */
}

