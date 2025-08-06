#include "HeaderCollection.h"
#include "FPhysX.h"

FPhysX* FPhysX::Instance = nullptr;

void FPhysX::Create()
{
    Assert(Instance == nullptr, "이미 FPhysX가 생성되었습니다.");
    Instance = new FPhysX();
    Instance->Initialize();
    
}

void FPhysX::Destroy()
{

    Delete(Instance);
}

void FPhysX::Initialize()
{
    
    Foundation = PxPtr<PxFoundation>::make_ptr(PxCreateFoundation(PX_PHYSICS_VERSION,
        DefaultAllocatorCallback, DefaultErrorCallback));
    Assert(Foundation.get() != nullptr, "Foundation Init Failed");
    
    Pvd = PxPtr<PxPvd>::make_ptr(PxCreatePvd(*Foundation));
    Transport = PxPtr<PxPvdTransport>::make_ptr(PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10));
    bool bConnected = Pvd->connect(*Transport.get(), physx::PxPvdInstrumentationFlag::eALL);
    if(!bConnected)
    {
        string log = "PhysX Visual Debugger Connect Failed";
        World::PushLog(log, Color(1, 0, 0, 1));
    }
    
    ToleranceScale.length = 100;
    ToleranceScale.speed = 980;
    Physics = PxPtr<PxPhysics>::make_ptr(PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation,
        ToleranceScale, true, Pvd.get()));
    Assert(Physics.get() != nullptr, "Physics Init Failed");

    
    PxSceneDesc sceneDesc(Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
    Dispatcher = PxPtr<PxDefaultCpuDispatcher>::make_ptr(PxDefaultCpuDispatcherCreate(7));
    sceneDesc.cpuDispatcher = Dispatcher.get();
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    PhysScene = PxPtr<PxScene>::make_ptr(Physics->createScene(sceneDesc));

    PxPvdSceneClient* pvdClient = PhysScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    ControllerManager = PxPtr<PxControllerManager>::make_ptr(PxCreateControllerManager(*PhysScene.get()));
}

void FPhysX::Simulate(float deltaTime)
{
    if (PhysScene)
    {
        PhysScene->simulate(deltaTime);
        PhysScene->fetchResults(true);
    }
}
