#include "Lighting.hlsli"



float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float4 PixelColor = float4(input.color, 1.0f);
    float3 dist = EyePos - input.posWorld;
    float3 toEye = normalize(dist);
    float distance = length(dist);
    float lod = ComputeLODBasedOnLog2(distance, BasicLodScale);
    float normalLod = ComputeLODBasedOnLog2(distance, NormalLod);
    float2 uv = input.texCoord * Material.UV_Tiling + Material.UV_Offset;
    float3 finalNormal = ApplyNormalMapping(input.texCoord, input.modelNormal, input.tangent, g_sampler, normalLod);
    
    
   
 

    float3 albedo = Material.useAlbedoMap ? MaterialMaps[TEXTURE_ALBEDO].SampleLevel(g_sampler, uv, lod).rgb * Material.Albedo.rgb
        : Material.Albedo.rgb;
    float ao = Material.useAOMap ? MaterialMaps[TEXTURE_AMBIENTOCCLUSION].SampleLevel(g_sampler, uv, lod).r : 1.0;
    float metallic = Material.useMetallicMap ? MaterialMaps[TEXTURE_METALLIC].SampleLevel(g_sampler, uv, lod).b * Material.Metallic
        : Material.Metallic;
    float roughness = Material.useRoughnessMap ? MaterialMaps[TEXTURE_ROUGHNESS].SampleLevel(g_sampler, uv, lod).g * Material.Roughness
        : Material.Roughness;
    float3 emission = Material.useEmissiveMap ? MaterialMaps[TEXTURE_EMISSIVE].SampleLevel(g_sampler, uv, lod).rgb
        : Material.Emissive;

    float3 ambientLighting = AmbientLightingByIBL(albedo, finalNormal, toEye, ao, metallic, roughness);

    PixelColor = float4(ambientLighting, 1.0);
    return PixelColor;

}