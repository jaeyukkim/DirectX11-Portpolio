#include "../Smoke.hlsli"
#include "../Sampler.hlsli"

Texture3D<float> densityTex : register(t0);
RWTexture3D<float> lightingTex : register(u0);


float BeerLambert(float absorptionCoefficient, float distanceTraveled)
{
    return exp(-absorptionCoefficient * distanceTraveled);
}

// �ڽ� �����ڸ� ��ǥ�κ��� 3D �ؽ��� ��ǥ ���
float3 GetUVW(float3 posModel)
{
    return (posModel.xyz + 1.0) * 0.5;
}

float LightRay(float3 posModel, float3 lightDir)
{
    // ��ó�� Ž��
    int numSteps = 128 / 4;
    float stepSize = 2.0 / float(numSteps);
    // float absorptionCoeff = 5.0;

    float alpha = 1.0; // visibility 1.0���� ����

    [loop] // [unroll] ��� �� ���̴� ������ �ʹ� ����
    for (int i = 0; i < numSteps; i++)
    {
        float prevAlpha = alpha;
        float density = densityTex.SampleLevel(LinearClampSampler, GetUVW(posModel), 0).
            r;

        if (density > 1e-3)
            alpha *= BeerLambert(lightAbsorptionCoeff * density, stepSize);

        posModel += -lightDir * stepSize;

        if (abs(posModel.x) > 1 || abs(posModel.y) > 1 || abs(posModel.z) > 1)
            break;

        if (alpha < 1e-3)
            break;
    }

    // alpha�� 0�� ����� ���� �������κ��� ���� �� �� ����
    return alpha;
}

[numthreads(16, 16, 4)]
void CS_Main(uint3 dtID : SV_DispatchThreadID)
{
    // float3 lightDir = float3(0, 1, 0);

    uint width, height, depth;
    lightingTex.GetDimensions(width, height, depth);

    float3 uvw = dtID / float3(width, height, depth); //+ uvwOffset; ����Ʈ���� �־��� �е��忡 ���� ����ϴ� ���̶� uvwOffset �̻��

    // uvw�� [0, 1]x[0,1]x[0,1]
    // �� ��ǥ��� [-1,1]x[-1,1]x[-1,1]
    lightingTex[dtID] = LightRay((uvw - 0.5) * 2.0, lightDir);
}
