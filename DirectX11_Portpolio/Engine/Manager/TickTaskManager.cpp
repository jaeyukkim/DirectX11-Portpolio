#include "HeaderCollection.h"
#include "TickTaskManager.h"
#include "Frameworks/USceneComponent.h"
#include "../../Game/System/ULevel.h"
#include "Frameworks/Actor.h"


FTickTaskManager* FTickTaskManager::Instance = nullptr;
bool FTickTaskManager::bNeedUpdate = true;  //첫 시작은 Tick 리스트를 업데이트 해야함

FTickTaskManager* FTickTaskManager::Get()
{
    assert(Instance != nullptr);

    return Instance;
}

void FTickTaskManager::Create(ULevel* Inlevel)
{
    assert(Instance == nullptr);
    Assert(Inlevel != nullptr, "Level이 유효하지 않습니다.");

    Instance = new FTickTaskManager();
    Instance->level = Inlevel;
}

void FTickTaskManager::Destroy()
{
}

void FTickTaskManager::LevelTick(float deltaTime)
{
    Instance->CalcDepthPerioity();
    for (shared_ptr<Actor>& actor : Instance->Actors)
    {
        actor->Tick(deltaTime);
    }
    for (UActorComponent* actorComp : Instance->ActorComponents)
    {
        actorComp->TickComponent(deltaTime);
    }
    for (USceneComponent* SceneComp : Instance->SceneComponents)
    {
        SceneComp->TickComponent(deltaTime);
    }

}

/*
 * 컴포넌트의 깂이 구조를 계산하는 함수
 * 레벨의 모든 액터와 컴포넌트를 가져와 다시 저장
 * SceneComponents는 AttachParent를 기준으로 Depth를 재귀적으로 계산
 * 계산된 Depth를 기반으로 정렬 후 SceneComponents에 오름차순 정렬
 */
void FTickTaskManager::CalcDepthPerioity()
{
    CheckFalse(bNeedUpdate);

    ResetAndRegisterAllData();

    vector<USceneComponent*> SortedComponents;
    unordered_set<USceneComponent*> Registered;

    for (USceneComponent* comp : SceneComponents)
    {
        RegisterComponentDepth(comp, Registered, SortedComponents);
    }
    
    
    std::sort(SortedComponents.begin(), SortedComponents.end(),
              [](USceneComponent* a, USceneComponent* b)
              {
                  return a->AttachDepth < b->AttachDepth;
              });
    

    
    SceneComponents = std::move(SortedComponents);
    bNeedUpdate = false;
}


void FTickTaskManager::ResetAndRegisterAllData()
{
    SceneComponents.clear();
    ActorComponents.clear();
    
    Actors = level->GetAllActor();
    
    for(shared_ptr<Actor>& actor : Actors)
    {
        vector<USceneComponent*>& SceneComps = actor->GetAllSceneComponents();
        vector<UActorComponent*>& ActorComps = actor->GetAllActorComponents();

        for(USceneComponent* scenecomp : SceneComps)
        {
            SceneComponents.push_back(scenecomp);
        }
        for(UActorComponent* actorcomp : ActorComps)
        {
            ActorComponents.push_back(actorcomp);
        }
    }
}



void FTickTaskManager::RegisterComponentDepth(USceneComponent* TargetComponent,
    unordered_set<USceneComponent*>& Registered, vector<USceneComponent*>& SortedComponents)
{
    USceneComponent* Root = TargetComponent;

    while (Root->AttachParent != nullptr)
    {
        Root = Root->AttachParent;
    }

    // 2. 루트부터 자식 순으로 재귀 등록
    function<void(USceneComponent*)> RegisterDepthRecursive = [&](USceneComponent* node)
    {
        if (Registered.find(node) != Registered.end())
            return;

        // Depth 설정
        if (node->AttachParent == nullptr)
            node->AttachDepth = 0;
        else
            node->AttachDepth = node->AttachParent->AttachDepth + 1;

        Registered.insert(node);
        SortedComponents.push_back(node);

        for (USceneComponent* child : node->AttachChildren)
        {
            RegisterDepthRecursive(child);
        }
    };

    RegisterDepthRecursive(Root);
}


