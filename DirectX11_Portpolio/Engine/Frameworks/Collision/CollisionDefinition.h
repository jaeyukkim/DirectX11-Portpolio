#pragma once


enum class ECollisionShape : uint8
{
    ECS_Sphere,
    ECS_Capsule,
    ECS_Box,
    ECS_Sqare
};
enum class EPhysicsType : uint8
{
    PT_Static = 0,
    PT_Dynamic = 1,
    PT_Kinematic = 2
};

enum class ECollisionDetectionType : uint8
{
    CDT_NoCollision = 0,
    CDT_QueryOnly = 1,
    CDT_PhysicsOnly = 2,
    CDT_AllEnabled = 3,

};
struct FPhysicsOption
{
    ECollisionDetectionType DetectionType = ECollisionDetectionType::CDT_AllEnabled;
    EPhysicsType PhysicsType = EPhysicsType::PT_Static;

    bool bUseController = false;
    bool bSimulatePhysics = false;
    bool bEnableGravity = false;
    bool bKinematic = false;
    bool GeneratedCollisionEvent = true;
    
    // ??? ???
    bool bLockPositionX = false;
    bool bLockPositionY = false;
    bool bLockPositionZ = false;

    // ??? ???
    bool bLockRotationX = false;
    bool bLockRotationY = false;
    bool bLockRotationZ = false;
    
};


struct FHitResult
{
    bool bBlockingHit = false;
    Vector3 HitLocation = Vector3::Zero;
    Vector3 TraceStart = Vector3::Zero;
    Vector3 TraceEnd = Vector3::Zero;
    
    Actor* HitActor;
};