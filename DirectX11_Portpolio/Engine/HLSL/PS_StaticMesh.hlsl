#include "Lighting.hlsli"


float4 PS_Main(VertexOutput input) : SV_TARGET
{
    float3 toEye = normalize(EyePos - input.posWorld);

    float3 color = float3(1.0f, 1.0f, 1.0f);

    int i = 0;


    for (int i = 0; i < LightCnt; ++i)
    {
        Light L = lights[i];
        color = L.color;
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

        case LIGHT_Lim:
            float rim = (1.0f - dot(input.modelNormal, toEye));
            rim = smoothstep(0.0f, 1.0f, rim);
            rim = pow(abs(rim), L.spotPower);
            color += rim * color * length(L.strength);
            break;
        }
    }

    return float4(color, 1.0f);


}