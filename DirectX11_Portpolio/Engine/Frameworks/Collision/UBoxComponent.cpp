#include "HeaderCollection.h"
#include "UBoxComponent.h"

#include "Render/FSceneRender.h"
#include "Render/Mesh/GeometryGenerator.h"

UBoxComponent::UBoxComponent(Vector3 InBoxSize, FPhysicsOption InOption)
    :UCollisionComponent(ECollisionShape::ECS_Box)
{
    BoxX = InBoxSize.x;
    BoxY = InBoxSize.y;
    BoxZ = InBoxSize.z;

    
    PhysicsOption = InOption;
    
}

void UBoxComponent::InitComponent()
{
    InitBox();
    InitPhysics();
   // FSceneRender::Get()->CreatePrimitiveRenderProxy(CollisionShape, this);

    Super::InitComponent();
}

void UBoxComponent::TickComponent(float deltaTime)
{
    UCollisionComponent::TickComponent(deltaTime);
    
    
}

void UBoxComponent::InitBox()
{
    
    StaticMeshData meshData = GeometryGenerator::MakeBox(Vector3(BoxX, BoxY, BoxZ));
    
    PrimData.AABB = std::move(meshData.AABB);
    PrimData.Indices = std::move(meshData.Indices);
    PrimData.Vertices = std::move(meshData.Vertices);
    
    PrimRenderData.VBuffer = make_shared<VertexBuffer>(PrimData.Vertices.data(), PrimData.Vertices.size(), sizeof(VertexObject));
    PrimRenderData.IndexCount = PrimData.Indices.size();
    PrimRenderData.IBuffer = make_shared<IndexBuffer>(PrimData.Indices.data(), PrimRenderData.IndexCount);
}

void UBoxComponent::InitPhysics()
{
    PxPhysics* physics = FPhysX::Get()->GetPhysics();
    PxScene* scene = FPhysX::Get()->GetPhysScene();
    PhysMaterial = PxPtr<PxMaterial>::make_ptr(
        physics->createMaterial(0.5f, 0.5f, 0.1f)); // 기본 마찰/탄성

    PxBoxGeometry boxGeom(BoxX/2, BoxY/2, BoxZ/2);
    

    // RigidBody 생성
    if (PhysicsOption.PhysicsType == EPhysicsType::PT_Static)
    {
        Vector3 pos = GetWorldTransform()->GetPosition();
        Quaternion quat = GetWorldTransform()->GetQuat();
        
        RigidStatic = PxPtr<PxRigidStatic>::make_ptr(
        physics->createRigidStatic(PxTransform(pos.x, pos.y, -pos.z,
            PxQuat(quat.x, quat.y,quat.z,quat.w))));

        PxPtr<PxShape> Shape = PxPtr<PxShape>::make_ptr(physics->createShape(boxGeom, *PhysMaterial.get()));
        RigidStatic->attachShape(*Shape);
        scene->addActor(*RigidStatic);
    }
    else // Dynamic 또는 Kinematic
    {
        RigidDynamic = PxPtr<PxRigidDynamic>::make_ptr(
            physics->createRigidDynamic(PxTransform(PxIdentity)));

        PxPtr<PxShape> Shape = PxPtr<PxShape>::make_ptr(physics->createShape(boxGeom, *PhysMaterial.get()));
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
