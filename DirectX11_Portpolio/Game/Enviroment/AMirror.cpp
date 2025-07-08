#include "Pch.h"
#include "AMirror.h"
#include "Frameworks/Components/UStaticMeshComponent.h"
#include "Render/FSceneRender.h"
#include "System/ULevel.h"


AMirror::AMirror()
{
    StaticMeshCreateInfo info;
    info.bIsMirror = true;

    
    mirror = CreateComponent<UStaticMeshComponent>(this, L"Floor");
    SetRootComponent(mirror.get());
    
    plane = SimpleMath::Plane(GetActorTransform()->Position, Vector3(-1.0f, 0.0f, 0.0f));
    ReflectRow = Matrix::CreateReflection(plane);

    FSceneRender::Get()->CreateRenderProxy<MirrorRenderProxy>(this);
}

void AMirror::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

}
