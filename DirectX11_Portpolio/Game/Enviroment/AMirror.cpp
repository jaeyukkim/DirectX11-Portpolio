#include "Pch.h"
#include "AMirror.h"
#include "Frameworks/UStaticMeshComponent.h"
#include "System/ULevel.h"
#include "Render/FSceneView.h"

AMirror::AMirror()
{
    mirror = CreateComponent<UStaticMeshComponent>(this, L"Floor");
    SetRootComponent(mirror.get());
    World::GetLevel()->AddToPostRenderActor(this);

    plane = SimpleMath::Plane(GetActorTransform()->Position, Vector3(-1.0f, 0.0f, 0.0f));
    ReflectRow = Matrix::CreateReflection(plane);
}

void AMirror::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

}

void AMirror::Render()
{
    //mirror->RenderComponent();
}

void AMirror::PostRender()
{
    mirror->RenderMirror(ReflectRow);
}
