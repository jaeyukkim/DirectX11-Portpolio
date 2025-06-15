#pragma once



class ULevel;
class Actor;
class UActorComponent;
class USceneComponent;


/*
 * 틱을 스케줄링 하고 실행하는 개체
 */
struct FTickTaskManager
{
public:
    static FTickTaskManager* Get();

public:
    static void Create(ULevel* Inlevel);
    static void Destroy();
    static void LevelTick(float deltaTime);

private:
    void ResetAndRegisterAllData();
    void CalcDepthPerioity();
    void RegisterComponentDepth(USceneComponent* TargetComponent,
                                unordered_set<USceneComponent*>& Registered,
                                vector<USceneComponent*>& SortedComponents);

public:
    static bool bNeedUpdate;
    
private:
    static FTickTaskManager* Instance;

private:
    FTickTaskManager() = default;
    ~FTickTaskManager() = default;

private:
    ULevel* level;
    vector<shared_ptr<Actor>> Actors;
    vector<UActorComponent*> ActorComponents;
    vector<USceneComponent*> SceneComponents;


};