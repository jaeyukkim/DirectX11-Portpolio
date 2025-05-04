#include "Lighting.hlsli"

float3 SchlickFresnel(float3 fresnelR0, float3 normal, float3 toEye)
{
    float normalDotView = saturate(dot(normal, toEye));
    float f0 = 1.0f - normalDotView; 
    return fresnelR0 + (1.0f - fresnelR0) * pow(f0, 5.0);
}


float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float3 toEye = EyePos - input.posWorld;
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

  
    
   
    float3 R = normalize(reflect(-toEye, finalNormal));

    float3 iblDiffuse = textureCube[MATERIAL_TEXTURE_Diffuse].Sample(g_sampler, finalNormal);
    float3 iblSpecular = textureCube[MATERIAL_TEXTURE_Specular].Sample(g_sampler, R);


    float3 specMap = MaterialMaps[MATERIAL_TEXTURE_Specular].Sample(g_sampler, uv).rgb;
    iblSpecular *= pow(saturate(dot(finalNormal, toEye)), Material.Shininess);
    iblSpecular *= specMap * Material.Specular.rgb;

    iblDiffuse *= Material.Diffuse.rgb;


    float3 baseDiffuse = MaterialMaps[MATERIAL_TEXTURE_Diffuse].Sample(g_sampler, uv).rgb;

    float3 finalColor = baseDiffuse * (color + iblDiffuse)+ iblSpecular;
    return float4(saturate(finalColor), 1.0f);


}