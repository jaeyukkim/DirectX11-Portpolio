#pragma once
#include "Frameworks/Objects/UObject.h"

class ACharacter;

class ULevel : public UObject
{
public:
    virtual void Initialize();
    virtual void Destroy();
    virtual void Tick(float deltaTime);

public:
    vector<shared_ptr<Actor>> GetAllActor();
    Actor* GetActor(int InObjectID);
    void AddActorToLevel(const shared_ptr<Actor>& InActor);
    void DestroyActor(Actor* InActor);
    
    ACharacter* GetPlayerCharacter() {return PlayerCharacter;}
    void SetPlayerCharacter(ACharacter* InPlayerCharacter) {PlayerCharacter = InPlayerCharacter;}
private:
    ACharacter* PlayerCharacter = nullptr;
    unordered_map<UINT32, shared_ptr<Actor>> Actors;
    static ULevel* level;
    static atomic<UINT32> ObjectCount;

    
};
