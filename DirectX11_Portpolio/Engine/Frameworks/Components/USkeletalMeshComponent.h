#pragma once

#include "UPrimitiveComponent.h"
#include "Frameworks/Animation/UAnimInstance.h"

class UAnimInstance;
class FClipData;
class Material;
class SkeletalMesh;
class Skeletal;

struct SkeletalMeshCreateInfo
{
    bool bOverWrite = false;
};


class USkeletalMeshComponent : public UPrimitiveComponent
{
public:
    USkeletalMeshComponent(wstring InMeshName);
    virtual ~USkeletalMeshComponent() override;
    virtual void TickComponent(float deltaTime) override;


public:
    vector<Material*> GetAllMaterials();
    vector<shared_ptr<SkeletalMesh>> GetAllMeshes() {return m_Mesh;}
    
    int GetInstanceID() {return *InstanceID;}
    void SetInstanceID(int* InInstanceId) {InstanceID = InInstanceId;}

    template<typename ClassType>
    void CreateAnimInstance(AnimInstanceCreateInfo info = AnimInstanceCreateInfo());
    void SetAnimInstace(shared_ptr<UAnimInstance> animInst) {AnimInstance = animInst;}
    UAnimInstance* GetAnimInstance() {return AnimInstance.get();}

    
private:
    vector<shared_ptr<Skeletal>> Bones;
    vector<shared_ptr<SkeletalMesh>> m_Mesh;
    map<string, shared_ptr<Material>> MaterialTable;
    unordered_map<string, shared_ptr<Skeletal>> ReadBoneTable;
    
    
    DirectX::SimpleMath::Matrix Transforms[MAX_MODEL_TRANSFORM];
    int* InstanceID;
    string MeshName;
    shared_ptr<UAnimInstance> AnimInstance;

    
private:
    friend class Converter;
    friend class UAnimInstance;
    friend class AnimationTexture;
};

template <typename ClassType>
void USkeletalMeshComponent::CreateAnimInstance(AnimInstanceCreateInfo info)
{
    static_assert(is_base_of_v<UAnimInstance, ClassType>, 
                  "CreateAnimInstance의 ClassType UAnimInstance로 부터 파생된 클래스여야 합니다");
    
    AnimInstance = make_shared<ClassType>(this, info);
}
 