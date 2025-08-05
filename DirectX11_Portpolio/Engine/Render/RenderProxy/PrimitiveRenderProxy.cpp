#include "HeaderCollection.h"
#include "PrimitiveRenderProxy.h"

PrimitiveRenderProxy::PrimitiveRenderProxy(UPrimitiveComponent* primComp)
    :RenderProxy(ERenderProxyType::RPT_Primitive),
    Append(nullptr, sizeof(FPrimitiveInstCPU), 0),
    Consume(nullptr, sizeof(FPrimitiveInstCPU), 0),
    CSIndirectBuffer(nullptr, 0, 0),
    InstanceIndirectBuffer(nullptr, sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS), 1)
{
    //반드시 해야함
    InstanceDatas.reserve(MAX_INSTANCE_SIZE);
    
    RenderData = std::move(primComp->PrimRenderData);

    minAABB = primComp->PrimData.AABB.Min;
    maxAABB = primComp->PrimData.AABB.Max;
    AddInstance(primComp);
}

void PrimitiveRenderProxy::Render(const FRenderOption& option)
{

    RunFrustumCulling();
    
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->PrimitivePSO);
    CopyCntToIndirect();
    Append.VSSetSRV(EShaderResourceSlot::ERS_InstanceData);
    
    RenderData.VBuffer->IASetVertexBuffer();
    RenderData.IBuffer->IASetIndexBuffer();
    
    D3D::Get()->GetDeviceContext()->DrawIndexedInstancedIndirect(
        InstanceIndirectBuffer.GetBuffer().Get(), 0);
    
}

void PrimitiveRenderProxy::DeleteInstance(const int InstanceID)
{
    if (InstanceID < 0 || InstanceID >= InstanceDatas.size()) return;
    InstanceDatas.erase(InstanceDatas.begin() + InstanceID);

    //instanceID 초기화
    for(int i = 0 ; i<InstanceDatas.size() ; i++)
    {
        InstanceDatas[i].InstanceID = i;
    }
    

    Consume = AppendBuffer(InstanceDatas.data(),
        sizeof(FPrimitiveInstCPU), InstanceDatas.size());
    Append = AppendBuffer(nullptr, sizeof(FPrimitiveInstCPU), InstanceDatas.size());
 
    CreateCSIndirectData();
    CteateInstanceIndirectData();
}

void PrimitiveRenderProxy::AddInstance(UPrimitiveComponent* primComp)
{
    if (InstanceDatas.size() > MAX_INSTANCE_SIZE)
    {
        Assert(true, "최대 Instance 수를 초과하였습니다.");
    }

    FPrimitiveInstCPU data;
    data.ModelMat = primComp->GetWorldBufferData()->World;
    data.AABB_Max = maxAABB;
    data.AABB_Min = minAABB;
    data.InstanceID = InstanceDatas.size();
    InstanceDatas.push_back(data);
    
    primComp->SetInstanceID(&InstanceDatas.back().InstanceID);

    Consume = AppendBuffer(InstanceDatas.data(),
       sizeof(FPrimitiveInstCPU), InstanceDatas.size());

    //초기 사이즈는 지정해줘야한다고??
    Append = AppendBuffer(nullptr, sizeof(FPrimitiveInstCPU), InstanceDatas.size());

    
    primComp->TransformChanged.Add(this, &PrimitiveRenderProxy::TransformChange);

    CreateCSIndirectData();
    CteateInstanceIndirectData();
}


void PrimitiveRenderProxy::RunFrustumCulling()
{
    FGlobalPSO::Get()->BindPSO(FGlobalPSO::Get()->FrustumCullingPSO);
    
    Append = AppendBuffer(nullptr, sizeof(FPrimitiveInstCPU), InstanceDatas.size());
    Consume.UpdateSubResource();
  
    Consume.CSSetUAV(EUAV_Slot::USLOT_InstanceConsume, InstanceDatas.size());
    Append.CSSetUAV(EUAV_Slot::USLOT_InstanceAppend, 0);
    
    D3D::Get()->GetDeviceContext()->DispatchIndirect(CSIndirectBuffer.GetBuffer().Get(), 0);
    D3D::Get()->ComputeShaderBarrier();
}


void PrimitiveRenderProxy::TransformChange(int id, Matrix& mat)
{
    InstanceDatas[id].ModelMat = mat;
}


void PrimitiveRenderProxy::CopyCntToIndirect()
{
    
    // Append의 카운터 값만 IndirectBuffer의 InstanceCount 위치에 GPU → GPU로 복사
    // AppendUAV → IndirectBuffer 의 offset 4 바이트 위치 (InstanceCount)
    D3D::Get()->GetDeviceContext()->CopyStructureCount(InstanceIndirectBuffer.GetBuffer().Get(), 4, Append.GetUAV().Get());
    
}


void PrimitiveRenderProxy::CteateInstanceIndirectData()
{
    
    D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
    ZeroMemory(&args, sizeof(args));
    args.InstanceCount = 0;
    args.IndexCountPerInstance = RenderData.IndexCount;
    args.BaseVertexLocation=0;
    args.StartIndexLocation=0;
    args.StartInstanceLocation=0;
    
    InstanceIndirectBuffer = IndirectBuffer(&args,
    sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS), 1);
}


void PrimitiveRenderProxy::CreateCSIndirectData()
{
    D3D11_DISPATCH_INDIRECT_ARGS args;
    args.ThreadGroupCountX = InstanceDatas.size();
    args.ThreadGroupCountY = 1;
    args.ThreadGroupCountZ = 1;

    CSIndirectBuffer = IndirectBuffer(&args,
        sizeof(D3D11_DISPATCH_INDIRECT_ARGS), 1);
}
