#pragma once
#include "UObject.h"

struct FTransform;

class USceneComponent;

class Actor : public UObject
{
public:
	Actor();
	virtual ~Actor() override;
	
	virtual void Tick(float deltaTime);

public:
	void SetRootComponent(USceneComponent* InRootComponent);
	USceneComponent* GetRootComponent();

public:
	FTransform* GetActorTransform();

public:
	vector<UActorComponent*>& GetAllActorComponents() {return OwnedActorComponents;}
	void AddToOwnedActorComponents(UActorComponent* InComponent);
	vector<USceneComponent*>& GetAllSceneComponents() {return OwnedSceneComponents;}
	void AddToOwnedSceneComponents(USceneComponent* InComponent);

	
public:
	UObject* Outer = nullptr;
	USceneComponent* RootComponent = nullptr;
	vector<UActorComponent*> OwnedActorComponents;
	vector<USceneComponent*> OwnedSceneComponents;
};