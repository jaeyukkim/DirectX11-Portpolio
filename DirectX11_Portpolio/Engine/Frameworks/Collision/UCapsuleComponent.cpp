#include "HeaderCollection.h"
#include "UCapsuleComponent.h"

#include "Render/FSceneRender.h"
#include "Render/Mesh/GeometryGenerator.h"

UCapsuleComponent::UCapsuleComponent(FPhysicsOption InOption)
    :UCollisionComponent(ECollisionShape::ECS_Capsule)
{
    PhysicsOption = InOption;

    InitCapsule();
    
    if(!InOption.bUseController)
    {
        InitPhysics();
    }
    FSceneRender::Get()->CreatePrimitiveRenderProxy(CollisionShape, this);
}
void UCapsuleComponent::TickComponent(float deltaTime)
{
    Super::TickComponent(deltaTime);
    
}


void UCapsuleComponent::InitPhysics()
{
    PxPhysics* physics = FPhysX::Get()->GetPhysics();
    PxScene* scene = FPhysX::Get()->GetPhysScene();
    PhysMaterial = PxPtr<PxMaterial>::make_ptr(
        physics->createMaterial(0.5f, 0.5f, 0.1f)); // 기본 마찰/탄성

    PxCapsuleGeometry CapsuleGeom(CapsuleRadius, CapsuleHalfHeight);
    

    // RigidBody 생성
    if (PhysicsOption.PhysicsType == EPhysicsType::PT_Static)
    {
        RigidStatic = PxPtr<PxRigidStatic>::make_ptr(
            physics->createRigidStatic(PxTransform(PxIdentity))
        );

        PxPtr<PxShape> Shape = PxPtr<PxShape>::make_ptr(physics->createShape(CapsuleGeom, *PhysMaterial.get()));
        RigidStatic->attachShape(*Shape);
        scene->addActor(*RigidStatic);
    }
    else // Dynamic 또는 Kinematic
    {
        RigidDynamic = PxPtr<PxRigidDynamic>::make_ptr(
            physics->createRigidDynamic(PxTransform(PxIdentity)));

        PxPtr<PxShape> Shape = PxPtr<PxShape>::make_ptr(physics->createShape(CapsuleGeom, *PhysMaterial.get()));
        RigidDynamic->attachShape(*Shape);
        
        // 질량 및 관성 설정
        PxRigidBodyExt::updateMassAndInertia(*RigidDynamic, 1.0f);

        if (PhysicsOption.PhysicsType == EPhysicsType::PT_Kinematic)
        {
            RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        }

        // 중력 사용 여부
        RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !PhysicsOption.bEnableGravity);

        
        // 축 잠금 적용
        PxRigidDynamicLockFlags LockFlags;
        if (PhysicsOption.bLockPositionX) LockFlags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
        if (PhysicsOption.bLockPositionY) LockFlags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
        if (PhysicsOption.bLockPositionZ) LockFlags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
        if (PhysicsOption.bLockRotationX) LockFlags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
        if (PhysicsOption.bLockRotationY) LockFlags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
        if (PhysicsOption.bLockRotationZ) LockFlags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
        RigidDynamic->setRigidDynamicLockFlags(LockFlags);
        
        scene->addActor(*RigidDynamic);
    }

    
}


void UCapsuleComponent::InitCapsule()
{
    Super::CreatePrimitive();
    
    StaticMeshData meshData = GeometryGenerator::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
    
    PrimData.AABB = std::move(meshData.AABB);
    PrimData.Indices = std::move(meshData.Indices);
    PrimData.Vertices = std::move(meshData.Vertices);
    
    PrimRenderData.VBuffer = make_shared<VertexBuffer>(PrimData.Vertices.data(), PrimData.Vertices.size(), sizeof(VertexObject));
    PrimRenderData.IndexCount = PrimData.Indices.size();
    PrimRenderData.IBuffer = make_shared<IndexBuffer>(PrimData.Indices.data(), PrimRenderData.IndexCount);
}
