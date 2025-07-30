#pragma once


#include "Frameworks/Components/USceneComponent.h"

struct FViewContext;

class UCameraComponent : public USceneComponent
{
public:
	UCameraComponent();
	virtual ~UCameraComponent() override;

	virtual void TickComponent(float deltaTime) override;

public:
	FViewContext& GetViewMatrix() { return ViewContext; }
	void AddLookInput(Vector2 InMoveDelta);
public:
	void SetMoveSpeed(float InValue) { MoveSpeed = InValue; }
	void SetRotationSpeed(float InValue) { RotationSpeed = InValue; }
	

private:
	void SetViewMatrix();
	void CaptureCinematic(float deltaTime);

private:
	float MoveSpeed = 800.f;
	//float MoveSpeed = 2.f;

	float RotationSpeed = 25.f;
	float aspect = 16.0f / 9.0f;
	float FOV = 75.0f;
	float FrustumFOV = 90.0f;

private:
	FViewContext ViewContext;
	FViewContext FrustumViewContext;

};
