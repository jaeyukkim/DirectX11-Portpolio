#pragma once
#include "Frameworks/UObject.h"


class ULevel : public UObject
{
public:
    virtual void Initialize();
    virtual void Destroy();
    virtual void Tick(float deltaTime);
    virtual void Render();


public:
    vector<shared_ptr<Actor>> GetAllActor();
    Actor* GetActor(int InObjectID);
    void AddActorToLevel(const shared_ptr<Actor>& InActor);
    void DestroyActor(Actor* InActor);
private:
    unordered_map<UINT32, shared_ptr<Actor>> Actors;
    static ULevel* level;
    static atomic<UINT32> ObjectCount;
};
