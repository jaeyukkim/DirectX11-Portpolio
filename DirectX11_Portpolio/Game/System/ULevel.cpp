#include "Pch.h"
#include "ULevel.h"
#include "AKachujin.h"
#include "ALightActor.h"
#include "Enviroment/AFloor.h"
#include "Enviroment/ACubeMap.h"
#include "Enviroment/AMirror.h"

atomic<UINT32> ULevel::ObjectCount = -1;

void ULevel::Initialize()
{
    FTickTaskManager::Create(this);


    SpawnActor<ALightActor>(this);
    SpawnActor<AKachujin>(this);
    SpawnActor<AFloor>(this);
    SpawnActor<ACubeMap>(this);
    //SpawnActor<AMirror>(this);
}

void ULevel::Destroy()
{
    FTickTaskManager::Destroy();
}

void ULevel::Tick(float deltaTime)
{
    FTickTaskManager::LevelTick(deltaTime);
}



vector<shared_ptr<Actor>> ULevel::GetAllActor()
{
    vector<shared_ptr<Actor>> allActor;
    allActor.reserve(Actors.size());

    for (const auto& [id, actor] : Actors)
    {
        if (actor) // nullptr 방어
            allActor.push_back(actor);
    }

    return allActor;

}

/**
* @param InObjectID : 해당 Actor의 고유ID를 전달
*/
Actor* ULevel::GetActor(int InObjectID)
{
    auto it = Actors.find(InObjectID);
    if (it != Actors.end())
        return it->second.get();
    return nullptr;
}


void ULevel::AddActorToLevel(const shared_ptr<Actor>& InActor)
{
    CheckNull(InActor)

    InActor->ObjectID = ++ObjectCount;
    Actors.insert({ ObjectCount,  InActor });
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