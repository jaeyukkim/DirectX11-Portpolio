#pragma once


class FPhysX
{
public:
    static void Create();
    static void Destroy();
    static FPhysX* Get() { return Instance; }
    void Initialize();

    PxPhysics* GetPhysics() {return Physics.get();}
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
    PxPvdTransport* Transport;
    
    static FPhysX* Instance;
};
