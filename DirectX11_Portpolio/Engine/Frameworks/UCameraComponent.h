#pragma once

#include "USceneComponent.h"
#include "Render/FSceneView.h"

struct FViewContext;

class UCameraComponent : public USceneComponent
{
public:
	UCameraComponent();
	virtual ~UCameraComponent() override;

	virtual void TickComponent(float deltaTime) override;

public:
	FViewContext& GetViewMatrix() { return ViewContext; }

public:
	void SetMoveSpeed(float InValue) { MoveSpeed = InValue; }
	void SetRotationSpeed(float InValue) { RotationSpeed = InValue; }

private:
	void SetViewMatrix();
	

private:
	float MoveSpeed = 20.f;
	float RotationSpeed = 250.f;
	float aspect = 0.f;
	

private:
	FViewContext ViewContext;
};
