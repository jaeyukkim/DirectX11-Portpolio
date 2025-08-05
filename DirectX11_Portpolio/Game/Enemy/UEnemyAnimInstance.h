#pragma once


class AEnemy;
class UEnemyAnimInstance : public UAnimInstance
{
public:
    UEnemyAnimInstance(USkeletalMeshComponent* meshComp);
    virtual ~UEnemyAnimInstance() = default;

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float deltaTime);
 
    void CreateNode();

private:
    AEnemy* m_Enemy = nullptr;
};
