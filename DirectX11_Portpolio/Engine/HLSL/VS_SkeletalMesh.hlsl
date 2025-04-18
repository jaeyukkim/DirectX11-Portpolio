#include "Common.hlsli"



#define MAX_MODEL_TRANSFORM 250
cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;
};




VertexOutput VS_Main(VertexShaderInput input)
{
    // ��(Model) ����� �� �ڽ��� �������� 
    // ���� ��ǥ�迡���� ��ġ�� ��ȯ�� �����ݴϴ�.
    // �� ��ǥ���� ��ġ -> [�� ��� ���ϱ�] -> ���� ��ǥ���� ��ġ
    // -> [�� ��� ���ϱ�] -> �� ��ǥ���� ��ġ -> [�������� ��� ���ϱ�]
    // -> ��ũ�� ��ǥ���� ��ġ

    // �� ��ǥ��� NDC�̱� ������ ���� ��ǥ�� �̿��ؼ� ���� ���

    VertexOutput output;


    float4 pos = float4(input.posModel, 1.0f); // �� ���� ��ġ
    matrix skinWorld = mul(BoneTransforms[BoneIndex], World); // ��Ų ���� ���

    float4 worldPos = mul(pos, skinWorld);     // ���� ���� ��ġ
    output.posWorld = worldPos.xyz;            // ���⼭ float3 �����ؼ� ����

    // ���� ȭ�� ���� ��ȯ
    output.posProj = mul(worldPos, View);
    output.posProj = mul(output.posProj, Projection);

    output.normalWorld = mul(input.modelNormal, (float3x3)skinWorld);

    output.texCoord = input.texCoord;
    output.color = input.modelColor;
    return output;
}