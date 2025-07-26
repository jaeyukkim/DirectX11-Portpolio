#pragma once


class FPhysX
{
public:
    static void Create();
    static void Destroy();
    static FPhysX* Get() { return Instance; }
    void Initialize();
    virtual void Simulate(float deltaTime);

    PxScene* GetPhysScene() {return PhysScene.get();}
    PxPhysics* GetPhysics() {return Physics.get();}
    PxControllerManager* GetControllerManager() { return ControllerManager.get();}

    
private:
    FPhysX() = default;
    ~FPhysX() = default;


private:
    PxDefaultAllocator	DefaultAllocatorCallback;
    PxDefaultErrorCallback	DefaultErrorCallback;
    PxPtr<PxFoundation>		Foundation;
    PxPtr<PxPhysics>		Physics;
    PxTolerancesScale	ToleranceScale;
    PxPtr<PxDefaultCpuDispatcher>	Dispatcher;
    PxPtr<PxScene>		PhysScene;
    PxPtr<PxMaterial>		Material;
    PxPtr<PxPvd>			Pvd;
    PxPtr<PxControllerManager> ControllerManager;
    PxPtr<PxPvdTransport> Transport;
    
    static FPhysX* Instance;
};
