#pragma once
#include "UActorComponent.h"




class USceneComponent : public UActorComponent
{
public:
	USceneComponent();
	virtual ~USceneComponent() = default;

	
	virtual void TickComponent(float deltaTime) override;
	virtual void RenderComponent();

public:
	void SetUpAttachment(USceneComponent* InParent, const string& InSocketName = "");
	void Deteach();
	void AddChild(USceneComponent* InChild);
	void RemoveChild(USceneComponent* InChild);
	
public:
	FTransform* GetWorldTransform() const { return WorldTransform.get(); }
	FTransform* GetRelativeTransform() const { return RelativeTransform.get(); }

public:
	vector<USceneComponent*> AttachChildren;
	USceneComponent* AttachParent = nullptr;
	string AttachSocketName;

public:
	bool bDirty = false;
	int AttachDepth = 0;
	Vector3 CachedEulerRotation;
	

private:
	struct WorldBufferDesc
	{
		Matrix World;
	} WorldBufferData;

	shared_ptr<FTransform> WorldTransform;
	shared_ptr<FTransform> RelativeTransform;
	shared_ptr<ConstantBuffer> WorldConstantBuffer;
	
	
};