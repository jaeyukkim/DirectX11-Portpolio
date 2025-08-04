#include "HeaderCollection.h"
#include "USpringArmConponent.h"

USpringArmConponent::USpringArmConponent()
{
}

void USpringArmConponent::TickComponent(float deltaTime)
{
    if (AttachParent == nullptr)
    {
        WorldTransform->SetTransform(*RelativeTransform.get());
        WorldBufferData.World = WorldTransform->ToMatrix().Transpose();
    }
    else
    {
        //Spring Arm이 컨트롤러의 회전을 따르는지
        if (bUsePawnControlRotation)
        {
            Matrix WorldMat = RelativeTransform->ToMatrix() * AttachParent->GetWorldTransform()->ToMatrix();
            WorldTransform->SetTransformFromMatrix(WorldMat);
        }
  
        else
        {
            Matrix LocalMat = RelativeTransform->ToMatrix();
            FTransform* ParentTransform = AttachParent->GetWorldTransform();
            Vector3 parentScale = ParentTransform->GetScale();
            Vector3 parentPos   = ParentTransform->GetPosition();
      
            Matrix ParentScale = Matrix::CreateScale(parentScale);
            Matrix ParentTrans = Matrix::CreateTranslation(parentPos);
            Matrix ParentNoRot = ParentScale * ParentTrans;
            
            Matrix WorldMat = LocalMat * ParentNoRot;
            WorldTransform->SetTransformFromMatrix(WorldMat);
        }

        WorldBufferData.World = WorldTransform->ToMatrix().Transpose();
    }
}

void USpringArmConponent::AddLookInput(Vector2 InMoveDelta)
{
    FTransform* T = GetRelativeTransform();
    float pitch = InMoveDelta.y * RotationSpeed * Timer::Get()->GetDeltaTime();
    float yaw = InMoveDelta.x * RotationSpeed * Timer::Get()->GetDeltaTime();

    T->AddRotation(yaw, pitch, 0.0f);
}
