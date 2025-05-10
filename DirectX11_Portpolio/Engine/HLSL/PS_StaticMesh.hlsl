#include "Lighting.hlsli"



float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float4 PixelColor = float4(input.color, 1.0f);
    
    float3 toEye = normalize(EyePos - input.posWorld);
    float2 uv = input.texCoord * Material.UV_Tiling + Material.UV_Offset;
    float3 finalNormal = ApplyNormalMapping(uv, input.modelNormal, input.tangent, g_sampler);
    

    float3 albedo = Material.useAlbedoMap ? MaterialMaps[TEXTURE_ALBEDO].Sample(g_sampler, uv).rgb
        : Material.Albedo;
    float ao = Material.useAOMap ? MaterialMaps[TEXTURE_AMBIENTOCCLUSION].SampleLevel(g_sampler, uv, 0.0).r : 1.0;
    float metallic = Material.useMetallicMap ? MaterialMaps[TEXTURE_METALLIC].Sample(g_sampler, uv).r
        : Material.Metallic;
    float roughness = Material.useRoughnessMap ? MaterialMaps[TEXTURE_ROUGHNESS].Sample(g_sampler, uv).r
        : Material.Roughness;
    float3 emission = Material.useEmissiveMap ? MaterialMaps[TEXTURE_EMISSIVE].Sample(g_sampler, uv).rgb
        : float3(0, 0, 0);
    
    float3 ambientLighting = AmbientLightingByIBL(albedo, finalNormal, toEye, ao, metallic, roughness);

    //color = float4(ambientLighting + directLighting + emission, 1.0);
    PixelColor = float4(ambientLighting + emission, 1.0);

    return clamp(PixelColor, 0.0, 1000.0);


}