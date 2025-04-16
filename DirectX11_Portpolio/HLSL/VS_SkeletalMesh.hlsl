#include "Common.hlsli"
#include "Material.hlsl"


#define MAX_MODEL_TRANSFORM 250
cbuffer CB_ModelBones
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;
};


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
    float2 texcoord : TEXCOORD;
    float3 color : COLOR; // Normal lines ���̴����� ���
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
   
    
    World = mul(BoneTransforms[BoneIndex], World);
    
    output.posProj = mul(input.posProj, World);
    output.posProj = mul(output.posProj, View);
    output.posProj = mul(output.posProj, Projection);
    
    output.normalWorld = mul(input.Normal, (float3x3)World);
    
    output.texCoord = input.texCoord;
    output.color = input.color;
    return output;
}