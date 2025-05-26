#pragma once
#include "Frameworks/UObject.h"


class ULevel : public UObject
{
public:
    virtual void Initialize();
    virtual void Destroy();
    virtual void Tick(float deltaTime);
    virtual void Render();
    virtual void PostRender();


public:
    vector<shared_ptr<Actor>> GetAllActor();
    Actor* GetActor(int InObjectID);
    void AddActorToLevel(const shared_ptr<Actor>& InActor);
    void DestroyActor(Actor* InActor);
    void AddToPostRenderActor(Actor* InActor);
private:
    unordered_map<UINT32, shared_ptr<Actor>> Actors;
    vector<Actor*> PostRenderedActor;
    static ULevel* level;
    static atomic<UINT32> ObjectCount;
};
