#pragma once

#include "USceneComponent.h"

class UCameraComponent : public USceneComponent
{
public:
	UCameraComponent();
	virtual ~UCameraComponent();

	virtual void TickComponent(float deltaTime) override;

public:
	Matrix& GetViewMatrix() { return ViewMatrix; }

public:
	void SetMoveSpeed(float InValue) { MoveSpeed = InValue; }
	void SetRotationSpeed(float InValue) { RotationSpeed = InValue; }

private:
	void SetViewMatrix();
	

private:
	float MoveSpeed = 20.f;
	float RotationSpeed = 250.f;

private:
	Matrix ViewMatrix;
	Matrix PerspectiveProjection;
};