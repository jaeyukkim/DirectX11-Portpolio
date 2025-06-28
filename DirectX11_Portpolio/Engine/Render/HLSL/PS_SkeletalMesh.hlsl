#include "Lighting.hlsli"


float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float4 PixelColor = float4(input.color, 1.0f);
    float3 dist = EyePos - input.posWorld;
    float3 toEye = normalize(dist);
    float distance = length(dist);
    float lod = ComputeLODBasedOnLog2(distance, BasicLodScale);
    float normalLod = ComputeLODBasedOnLog2(distance, NormalLod);
    float2 uv = input.texCoord;
    float3 finalNormal = ApplyNormalMapping(input.texCoord, input.modelNormal, input.tangent, LinearWarpSampler, normalLod);


    float3 albedo = Material.useAlbedoMap ? MaterialMaps[TEXTURE_ALBEDO].SampleLevel(LinearWarpSampler, uv, lod).rgb * Material.Albedo.rgb
        : Material.Albedo.rgb;
    float ao = Material.useAOMap ? MaterialMaps[TEXTURE_AMBIENTOCCLUSION].SampleLevel(LinearWarpSampler, uv, lod).r : 1.0;
    float metallic = Material.useMetallicMap ? MaterialMaps[TEXTURE_METALLIC].SampleLevel(LinearWarpSampler, uv, lod).b * Material.Metallic
        : Material.Metallic;
    float roughness = Material.useRoughnessMap ? MaterialMaps[TEXTURE_ROUGHNESS].SampleLevel(LinearWarpSampler, uv, lod).g * Material.Roughness
        : Material.Roughness;
    float3 emission = Material.useEmissiveMap ? MaterialMaps[TEXTURE_EMISSIVE].SampleLevel(LinearWarpSampler, uv, lod).rgb
        : Material.Emissive;

    float3 ambientLighting = AmbientLightingByIBL(albedo, finalNormal, toEye, ao, metallic, roughness) * IBLStrength;

    float3 directLighting = float3(0, 0, 0);

    [unroll]
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        if(Lights[i].Type)
        {
            float3 lightVec = Lights[i].Type & LIGHT_Directional
                ? -Lights[i].direction
                : Lights[i].position - input.posWorld;

            float lightDist = length(lightVec);
            lightVec /= lightDist;
            float3 halfway = normalize(toEye + lightVec);

            float NdotI = max(0.0, dot(finalNormal, lightVec));
            float NdotH = max(0.0, dot(finalNormal, halfway));
            float NdotO = max(0.0, dot(finalNormal, toEye));

            float3 F0 = lerp(Fdielectric, albedo, metallic);
            float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, toEye)));
            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
            float3 diffuseBRDF = kd * albedo;

            float D = NdfGGX(NdotH, roughness);
            float3 G = SchlickGGX(NdotI, NdotO, roughness);
            float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);

            float3 radiance = LightRadiance(Lights[i], input.posWorld, finalNormal, ShadowMap[i]);

            directLighting += (Lights[i].Type & LIGHT_Lim) ? float3(0.0f, 0.0f, 0.0f) :
                ComputeLimLight(Lights[i], toEye, finalNormal);

            directLighting += (diffuseBRDF + specularBRDF) * NdotI * radiance;
        }
        
    }

    
        PixelColor = float4(ambientLighting + directLighting + emission, 1.0);
        PixelColor = clamp(PixelColor, 0.0, 1000.0);
        return PixelColor;
}