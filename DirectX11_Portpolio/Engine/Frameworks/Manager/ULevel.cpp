#include "HeaderCollection.h"
#include "ULevel.h"

#include "../../../Game/Enviroment/ACloud.h"
#include "../../../Game/Enviroment/AFluidActor.h"
#include "../Game/Characters/AKachujin.h"
#include "../Game/Enviroment/ALightActor.h"
#include "../Game/Characters/AEnemy.h"
#include "../Game/Enviroment/AFloor.h"
#include "../Game/Enviroment/ACubeMap.h"
#include "../Game/Enviroment/AMirror.h"
#include "Frameworks/AI/AAIController.h"
#include "Frameworks/Manager/AGameMode.h"

atomic<UINT32> ULevel::ObjectCount = -1;

void ULevel::Initialize()
{
    FTickTaskManager::Create(this);

    SpawnActor<AGameMode>(this);
    SpawnActor<ALightActor>(this);
    SpawnActor<ACubeMap>(this);

    FTransform transform;
    transform.SetPosition(0.0f, 200.0f, 300.0f);
    Actor* enemy = SpawnActorAtLocation<AEnemy>(World::GetLevel(), transform);
    Actor* aiController = UObject::SpawnActor<AAIController>(World::GetLevel());

    if(AEnemy* enemyCharacter = dynamic_cast<AEnemy*>(enemy))
    {
        if(AAIController* AIC = dynamic_cast<AAIController*>(aiController))
        {
            AIC->Possess(enemyCharacter);
        }
    }

    transform.SetPosition(0.0f, 0.0f, 0.0f);
    SpawnActorAtLocation<AFloor>(this, transform);
    
    //transform.SetPosition(-6000.0f, -100.0f, 0.0f);
    //SpawnActorAtLocation<AFluidActor>(this, transform);

    transform.SetPosition(0.0f, 5000.0f, 0.0f);
    SpawnActorAtLocation<ACloud>(this, transform);
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

void ULevel::DestroyActor(int InObjectID)
{
    
    auto it = Actors.find(InObjectID);
    if (it != Actors.end())
    {
        Actors.erase(it);
    }
    else
    {
        assert(false && "등록되어있지 않은 액터 제거");
    }

    FTickTaskManager::bNeedUpdate = true;
}