#include "HeaderCollection.h"
#include "UCameraComponent.h"
#include "Render/FSceneView.h"

UCameraComponent::UCameraComponent()
{
	ViewMatrix = Matrix::Identity;
	SetViewMatrix();
}

UCameraComponent::~UCameraComponent()
{
	
}

void UCameraComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);


	//CheckFalse(Mouse::Get()->Press(MouseButton::Right));

	FTransform* T = GetTransform();

	Vector3 pos = T->GetPosition();

	if (Keyboard::Get()->Press('W'))
		T->SetPosition(pos + T->GetForwardVector() * MoveSpeed * deltaTime);
	
	else if (Keyboard::Get()->Press('S'))
		T->SetPosition(pos - T->GetForwardVector() * MoveSpeed * deltaTime);

	if (Keyboard::Get()->Press('D'))
		T->SetPosition(pos - T->GetRightVector() * MoveSpeed * deltaTime);
	
	else if (Keyboard::Get()->Press('A'))
		T->SetPosition(pos - T->GetRightVector() * MoveSpeed * deltaTime);


	Vector3 delta = Mouse::Get()->GetMoveDelta();

	float pitch = delta.y * RotationSpeed * deltaTime;
	float yaw = delta.x * RotationSpeed * deltaTime;

	T->AddRotation(yaw, pitch, 0.0f);

	SetViewMatrix();

	
	FSceneView::Get()->UpdateSceneView(ViewMatrix, PerspectiveProjection);
}



void UCameraComponent::SetViewMatrix()
{
	FTransform* T = GetTransform();
	
	ViewMatrix = Matrix::CreateLookAt(T->GetPosition(),
		(T->GetPosition() + T->GetForwardVector()), T->GetUpVector());

	PerspectiveProjection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		D3D::Get()->GetDesc().Width / D3D::Get()->GetDesc().Height,
		0.1f, 1000.0f);
}


