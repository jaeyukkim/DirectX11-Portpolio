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
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float2 texCoord : TEXCOORD; // <- 다음 예제에서 사용
    float4 modelColor : COLOR;
    float3 modelNormal : NORMAL; // 모델 좌표계의 normal    
    float3 tanzent : TANGENT;
    float4 blendIndicies : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHTS;
    
    
};

struct VertexOutput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 color : COLOR; // Normal lines 쉐이더에서 사용
};


VertexOutput VS_Main(VertexShaderInput input)
{
    // 모델(Model) 행렬은 모델 자신의 원점에서 
    // 월드 좌표계에서의 위치로 변환을 시켜줍니다.
    // 모델 좌표계의 위치 -> [모델 행렬 곱하기] -> 월드 좌표계의 위치
    // -> [뷰 행렬 곱하기] -> 뷰 좌표계의 위치 -> [프로젝션 행렬 곱하기]
    // -> 스크린 좌표계의 위치

    // 뷰 좌표계는 NDC이기 때문에 월드 좌표를 이용해서 조명 계산

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