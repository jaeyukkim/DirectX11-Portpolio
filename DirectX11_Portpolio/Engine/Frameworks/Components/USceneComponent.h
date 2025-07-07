#pragma once
#include "UActorComponent.h"


struct WorldBufferDesc;
struct FTransform;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FWorldTransformChanged, int, Matrix&);

class USceneComponent : public UActorComponent
{
public:
	USceneComponent();
	virtual ~USceneComponent() = default;
	virtual void TickComponent(float deltaTime) override;

public:
	void SetUpAttachment(USceneComponent* InParent, const string& InSocketName = "");
	void Deteach();
	void AddChild(USceneComponent* InChild);
	void RemoveChild(USceneComponent* InChild);
	
public:
	FTransform* GetWorldTransform() const { return WorldTransform.get(); }
	FTransform* GetRelativeTransform() const { return RelativeTransform.get(); }
	shared_ptr<ConstantBuffer> GetWorldConstantBuffer() const {return WorldConstantBuffer; }
	WorldBufferDesc* GetWorldBufferData() { return &WorldBufferData;}
	
public:
	vector<USceneComponent*> AttachChildren;
	USceneComponent* AttachParent = nullptr;
	string AttachSocketName;
	
public:
	FWorldTransformChanged TransformChanged;
	bool bDirty = false;
	int AttachDepth = 0;
	

protected:
	
	WorldBufferDesc WorldBufferData;
	shared_ptr<FTransform> WorldTransform;
	shared_ptr<FTransform> RelativeTransform;
	shared_ptr<ConstantBuffer> WorldConstantBuffer;
	
	
};
