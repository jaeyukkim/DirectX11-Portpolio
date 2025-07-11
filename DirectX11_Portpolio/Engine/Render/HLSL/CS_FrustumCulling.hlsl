#include "Common.hlsli"


cbuffer CB_Frustum : register(b9)
{
    float4 FrustumPlane[6];
};

ConsumeStructuredBuffer<SM_Instance> InInstances : register(u0);
AppendStructuredBuffer<SM_Instance> OutInstances : register(u1);



[numthreads(1, 1, 1)]
void CS_Main(uint3 DTid : SV_DispatchThreadID)
{
    
    // 버퍼에서 한 인스턴스 가져옴 (ConsumeStructuredBuffer 사용)
    SM_Instance inst = InInstances.Consume();  
   
    
    // AABB의 8개 코너를 월드 좌표로 변환
    float3 corners[8];
    corners[0] = mul(float4(inst.AABB_Min.x, inst.AABB_Min.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[1] = mul(float4(inst.AABB_Max.x, inst.AABB_Min.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[2] = mul(float4(inst.AABB_Min.x, inst.AABB_Max.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[3] = mul(float4(inst.AABB_Max.x, inst.AABB_Max.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[4] = mul(float4(inst.AABB_Min.x, inst.AABB_Min.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;
    corners[5] = mul(float4(inst.AABB_Max.x, inst.AABB_Min.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;
    corners[6] = mul(float4(inst.AABB_Min.x, inst.AABB_Max.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;
    corners[7] = mul(float4(inst.AABB_Max.x, inst.AABB_Max.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;

    // 절두체 평면들과의 교차 테스트: 모든 코너가 한 평면의 바깥쪽(음수쪽)인지 검사
    bool isInside = true;
    [unroll]
    for (int p = 0; p < 6; p++)
    {
        bool allBehind = true;
        float4 plane = FrustumPlane[p];
        [unroll]
        for (int i = 0; i < 8; i++)
        {
            // 점-평면 거리 계산: plane.xyz·corner + plane.w
            float dist = dot(plane.xyz, corners[i]) + plane.w;
            
            if (dist >= 0) // (방향에 따라 부호가 바뀔 수 있음)
            { 
                allBehind = false;
                break;
            }
        }
        
        if (allBehind)
        { // 모든 점이 이 평면의 바깥쪽이면 컬링
            isInside = false;
            break;
        }
    }

    // 컬링되지 않은 인스턴스만 OutInstances에 추가(Append)
    if (isInside)
    {
        OutInstances.Append(inst);             // 생존한 인스턴스 저장
    }
    
}