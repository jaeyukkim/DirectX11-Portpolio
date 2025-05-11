#include "Lighting.hlsli"



float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float4 PixelColor = float4(input.color, 1.0f);
    float3 toEye = normalize(EyePos - input.posWorld);
    float3 finalNormal = ApplyNormalMapping(input.texCoord, input.modelNormal, input.tangent, g_sampler);
    
    
    float3 albedo = Material.useAlbedoMap ? MaterialMaps[TEXTURE_ALBEDO].SampleLevel(g_sampler, input.texCoord, 0).rgb * Material.Albedo.rgb
        : Material.Albedo.rgb;
    float ao = Material.useAOMap ? MaterialMaps[TEXTURE_AMBIENTOCCLUSION].SampleLevel(g_sampler, input.texCoord, 0).r : 1.0;
    float metallic = Material.useMetallicMap ? MaterialMaps[TEXTURE_METALLIC].SampleLevel(g_sampler, input.texCoord, 0).b * Material.Metallic
        : Material.Metallic;
    float roughness = Material.useRoughnessMap ? MaterialMaps[TEXTURE_ROUGHNESS].SampleLevel(g_sampler, input.texCoord, 0).g * Material.Roughness
        : Material.Roughness;
    float3 emission = Material.useEmissiveMap ? MaterialMaps[TEXTURE_EMISSIVE].SampleLevel(g_sampler, input.texCoord, 0).rgb
        : Material.Emissive;

    float3 ambientLighting = AmbientLightingByIBL(albedo, finalNormal, toEye, ao, metallic, roughness);

    PixelColor = float4(ambientLighting, 1.0);
    return PixelColor;
    
}

