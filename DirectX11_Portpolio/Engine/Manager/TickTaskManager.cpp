#include "HeaderCollection.h"
#include "TickTaskManager.h"
#include "Frameworks/USceneComponent.h"
#include "../../Game/System/ULevel.h"
#include "Frameworks/Actor.h"


FTickTaskManager* FTickTaskManager::Instance = nullptr;
bool FTickTaskManager::bNeedUpdate = true;  //ù ������ Tick ����Ʈ�� ������Ʈ �ؾ���

FTickTaskManager* FTickTaskManager::Get()
{
    assert(Instance != nullptr);

    return Instance;
}

void FTickTaskManager::Create(ULevel* Inlevel)
{
    assert(Instance == nullptr);
    Assert(Inlevel != nullptr, "Level�� ��ȿ���� �ʽ��ϴ�.");

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
 * ������Ʈ�� ���� ������ ����ϴ� �Լ�
 * ������ ��� ���Ϳ� ������Ʈ�� ������ �ٽ� ����
 * SceneComponents�� AttachParent�� �������� Depth�� ��������� ���
 * ���� Depth�� ������� ���� �� SceneComponents�� �������� ����
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

    // 2. ��Ʈ���� �ڽ� ������ ��� ���
    function<void(USceneComponent*)> RegisterDepthRecursive = [&](USceneComponent* node)
    {
        if (Registered.find(node) != Registered.end())
            return;

        // Depth ����
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


