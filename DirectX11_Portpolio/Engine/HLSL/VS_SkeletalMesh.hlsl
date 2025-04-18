#include "Common.hlsli"



#define MAX_MODEL_TRANSFORM 250
cbuffer CB_ModelBones : register(b3)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];

    uint BoneIndex;
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


    float4 pos = float4(input.posModel, 1.0f); // 모델 공간 위치
    matrix skinWorld = mul(BoneTransforms[BoneIndex], World); // 스킨 월드 행렬

    float4 worldPos = mul(pos, skinWorld);     // 월드 공간 위치
    output.posWorld = worldPos.xyz;            // 여기서 float3 추출해서 저장

    // 이후 화면 공간 변환
    output.posProj = mul(worldPos, View);
    output.posProj = mul(output.posProj, Projection);

    output.normalWorld = mul(input.modelNormal, (float3x3)skinWorld);

    output.texCoord = input.texCoord;
    output.color = input.modelColor;
    return output;
}