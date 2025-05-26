#include "HeaderCollection.h"
#include "Actor.h"
#include "USceneComponent.h"

Actor::Actor()
{
}

Actor::~Actor()
{
}

void Actor::Tick(float deltaTime)
{

}
void Actor::Render()
{

}
void Actor::PostRender()
{

}

void Actor::SetRootComponent(USceneComponent* InRootComponent)
{
    RootComponent = InRootComponent;
}

USceneComponent* Actor::GetRootComponent()
{
    return RootComponent;
}

FTransform* Actor::GetActorTransform()
{
    if(RootComponent == nullptr)
        return nullptr;

    return GetRootComponent()->GetRelativeTransform();
}

void Actor::AddToOwnedActorComponents(UActorComponent* InComponent)
{
    //auto it = std::find(OwnedActorComponents.begin(), OwnedActorComponents.end(), InComponent);
    //assert(it == OwnedActorComponents.end() && "�̹� ���Ϳ� ��� �� ���� ������Ʈ �Դϴ�");

    OwnedActorComponents.push_back(InComponent);
    FTickTaskManager::bNeedUpdate = true;
}

void Actor::AddToOwnedSceneComponents(USceneComponent* InComponent)
{
    
   // auto it = std::find(OwnedSceneComponents.begin(), OwnedSceneComponents.end(), InComponent);
   // assert(it == OwnedSceneComponents.end() && "�̹� ���Ϳ� ��� �� �� ������Ʈ �Դϴ�");

    OwnedSceneComponents.push_back(InComponent);
    FTickTaskManager::bNeedUpdate = true;
}
