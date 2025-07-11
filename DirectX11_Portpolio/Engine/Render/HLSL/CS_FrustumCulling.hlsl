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
    
    // ���ۿ��� �� �ν��Ͻ� ������ (ConsumeStructuredBuffer ���)
    SM_Instance inst = InInstances.Consume();  
   
    
    // AABB�� 8�� �ڳʸ� ���� ��ǥ�� ��ȯ
    float3 corners[8];
    corners[0] = mul(float4(inst.AABB_Min.x, inst.AABB_Min.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[1] = mul(float4(inst.AABB_Max.x, inst.AABB_Min.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[2] = mul(float4(inst.AABB_Min.x, inst.AABB_Max.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[3] = mul(float4(inst.AABB_Max.x, inst.AABB_Max.y, inst.AABB_Min.z, 1.0), inst.Inst_World).xyz;
    corners[4] = mul(float4(inst.AABB_Min.x, inst.AABB_Min.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;
    corners[5] = mul(float4(inst.AABB_Max.x, inst.AABB_Min.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;
    corners[6] = mul(float4(inst.AABB_Min.x, inst.AABB_Max.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;
    corners[7] = mul(float4(inst.AABB_Max.x, inst.AABB_Max.y, inst.AABB_Max.z, 1.0), inst.Inst_World).xyz;

    // ����ü ������� ���� �׽�Ʈ: ��� �ڳʰ� �� ����� �ٱ���(������)���� �˻�
    bool isInside = true;
    [unroll]
    for (int p = 0; p < 6; p++)
    {
        bool allBehind = true;
        float4 plane = FrustumPlane[p];
        [unroll]
        for (int i = 0; i < 8; i++)
        {
            // ��-��� �Ÿ� ���: plane.xyz��corner + plane.w
            float dist = dot(plane.xyz, corners[i]) + plane.w;
            
            if (dist >= 0) // (���⿡ ���� ��ȣ�� �ٲ� �� ����)
            { 
                allBehind = false;
                break;
            }
        }
        
        if (allBehind)
        { // ��� ���� �� ����� �ٱ����̸� �ø�
            isInside = false;
            break;
        }
    }

    // �ø����� ���� �ν��Ͻ��� OutInstances�� �߰�(Append)
    if (isInside)
    {
        OutInstances.Append(inst);             // ������ �ν��Ͻ� ����
    }
    
}