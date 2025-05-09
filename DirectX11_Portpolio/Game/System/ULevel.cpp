#include "Pch.h"
#include "ULevel.h"
#include "AKachujin.h"
#include "ALightActor.h"
#include "Enviroment/AFloor.h"
#include "Enviroment/ACubeMap.h"

void ULevel::Initialize()
{
    FTickTaskManager::Create(this);


//    SpawnActor<ALightActor>(this);
    SpawnActor<AKachujin>(this);
//    SpawnActor<AFloor>(this);
//    SpawnActor<ACubeMap>(this);
}

void ULevel::Destroy()
{
    FTickTaskManager::Destroy();
}

void ULevel::Tick(float deltaTime)
{
    FTickTaskManager::LevelTick(deltaTime);
}

void ULevel::Render()
{
    for (shared_ptr<Actor>& actor : Actors)
    {
        actor->Render();
    }
}

void ULevel::AddActorToLevel(const shared_ptr<Actor>& InActor)
{
   /* auto it = std::find(Actors.begin(), Actors.end(), InActor);
    assert(it == Actors.end() && "이미 레벨에 등록 된 액터 입니다");*/

    Actors.push_back(InActor);
    FTickTaskManager::bNeedUpdate = true;
}

void ULevel::DestroyActor(Actor* InActor)
{
    assert(InActor != nullptr && "Actor is null");

    /*
    auto it = std::find(Actors.begin(), Actors.end(), InActor);
    if (it != Actors.end())
    {
        Actors.erase(it);
    }
    else
    {
        assert(false && "등록되어있지 않은 액터 제거");
    }*/
    FTickTaskManager::bNeedUpdate = true;
}