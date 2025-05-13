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
	float MoveSpeed = 10.f;
	float RotationSpeed = 25.f;
	float aspect = 16.0f / 9.0f;
	

private:
	FViewContext ViewContext;
};
