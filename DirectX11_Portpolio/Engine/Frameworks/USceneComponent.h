#pragma once
#include "UActorComponent.h"




class USceneComponent : public UActorComponent
{
public:
	USceneComponent();
	virtual ~USceneComponent();

	
	virtual void TickComponent(float deltaTime) override;
	virtual void RenderComponent();

public:
	void SetUpAttachment(USceneComponent* InParent, const string& InSocketName);
	void AddChild(USceneComponent* Child);
	void RemoveChild(USceneComponent* Child);
	
public:
	FTransform* GetTransform() { return Transform.get(); }
	
	
private:
	struct WorldBufferDesc
	{
		Matrix World;
	} WorldBufferData;

	shared_ptr<FTransform> Transform;
	shared_ptr<ConstantBuffer> WorldConstantBuffer;


private:
	vector<USceneComponent*> AttachChildren;
	USceneComponent* AttachParent;
	string AttachSocketName;

private:
	bool bDirty = false;
};