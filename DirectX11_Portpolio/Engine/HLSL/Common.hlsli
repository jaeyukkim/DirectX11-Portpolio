#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#include "Material.hlsli"
// ���̴����� include�� ������� .hlsli ���Ͽ� �ۼ�
// Properties -> Item Type: Does not participate in build���� ����

// BlinnPhong ������ ��ü ������ Luna DX12 ����� ��������� 
// ���� ������ �����ϱ� ���ϵ��� ���� ���� ��Ÿ�Ϸ� �ܼ�ȭ�Ͽ����ϴ�.



cbuffer CB_World : register(b1)
{
    matrix World;
};

cbuffer CB_ViewContext : register(b2)
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix ViewProjection;

}



// ����
struct Light
{
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};



float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
    float3 toEye, MaterialDesc mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 Specular = mat.Specular * pow(max(hdotn, 0.0f), 1.0f);

    return mat.Ambient + (mat.Diffuse + Specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, MaterialDesc mat, float3 normal,
    float3 toEye)
{
    float3 lightVec = -L.direction;

    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;

    // Luna DX12 å������ Specular ��꿡��
    // Lambert's law�� ����� lightStrength�� ����մϴ�.
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 ComputePointLight(Light L, MaterialDesc mat, float3 pos, float3 normal,
    float3 toEye)
{
    float3 lightVec = L.position - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= d;

        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;

        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, MaterialDesc mat, float3 pos, float3 normal,
    float3 toEye)
{
    float3 lightVec = L.position - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= d;

        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;

        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        float spotFactor = pow(max(-dot(lightVec, L.direction), 0.0f), L.spotPower);
        lightStrength *= spotFactor;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }

}

struct VertexShaderInput
{
    float3 posModel : POSITION; //�� ��ǥ���� ��ġ position
    float2 texCoord : TEXCOORD; // <- ���� �������� ���
    float4 modelColor : COLOR;
    float3 modelNormal : NORMAL; // �� ��ǥ���� normal    
    float3 tanzent : TANGENT;
    float4 blendIndicies : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHTS;

};

struct VertexOutput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (���� ��꿡 ���)
    float3 normalWorld : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR; // Normal lines ���̴����� ���
};

#endif // __COMMON_HLSLI__