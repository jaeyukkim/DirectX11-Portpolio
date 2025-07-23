#pragma once
#include "RenderProxy.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnBlendDataUpdate, int, FAnimBlendingData&);
class SkeletalMeshRenderProxy;


class AnimationRenderProxy : public RenderProxy
{
public:
    AnimationRenderProxy(UAnimInstance* animInst);
 
    void Render(const FRenderOption& option) override;
    
    void AddInstance(UAnimInstance* animInst);
    void DeleteInstance(int InInstanceID);
    void SetBlendData(int InInstanceID, FAnimBlendingData& data);
   
    unordered_map<int, FAnimBlendingData> BlendingData;
    
    ComPtr<ID3D11Texture2D> ClipTexture = nullptr;
    ComPtr<ID3D11ShaderResourceView> ClipsSRV = nullptr;
    vector<shared_ptr<FClipData>> Animations;

private:
    FOnBlendDataUpdate BlendDataUpdate;
    friend class SkeletalMeshRenderProxy;
};
