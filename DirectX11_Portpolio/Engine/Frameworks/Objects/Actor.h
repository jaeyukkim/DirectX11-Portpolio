#pragma once
#include "UObject.h"

struct FTransform;
class UActorComponent;
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
	FTransform* GetActorTransform();
	virtual void InitAllComponents();
	vector<UActorComponent*>& GetAllActorComponents() {return OwnedActorComponents;}
	void AddToOwnedActorComponents(UActorComponent* InComponent);
	vector<USceneComponent*>& GetAllSceneComponents() {return OwnedSceneComponents;}
	void AddToOwnedSceneComponents(USceneComponent* InComponent);
	virtual void CustomRender(float deltaTime);
	
public:
	UObject* Outer = nullptr;
	USceneComponent* RootComponent = nullptr;
	vector<UActorComponent*> OwnedActorComponents;
	vector<USceneComponent*> OwnedSceneComponents;
};