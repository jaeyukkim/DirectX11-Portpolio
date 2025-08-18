cbuffer SmokeConst : register(b13)
{
    float3 uvwOffset;
    float lightAbsorptionCoeff = 5.0;
    float3 lightDir = float3(0, 1, 0);
    float densityAbsorption = 10.0;
    float3 lightColor = float3(1, 1, 1) * 40.0;
    float aniso = 0.3;
}

cbuffer CB_WorldInv : register(b12)
{
    float BoxHalfWidth;
    float3 WorldInvPad;
    matrix WorldInv;
};

float3 GetBoxUVW(float3 posBox, float halfWidth)
{
    float invHalf = 1.0 / max(halfWidth, 1e-6);
    float3 uvw = posBox * invHalf * 0.5 + 0.5; // [-1,1] -> [0,1]
    return saturate(uvw); // 범위 넘어가면 잘라줌
}