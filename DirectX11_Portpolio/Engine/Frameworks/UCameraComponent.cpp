#include "HeaderCollection.h"
#include "UCameraComponent.h"


UCameraComponent::UCameraComponent()
{
	FTransform* T = GetWorldTransform();
	SetViewMatrix();
}

UCameraComponent::~UCameraComponent()
{
	
}

void UCameraComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	CheckFalse(Mouse::Get()->Press(MouseButton::Right));

	FTransform* T = GetRelativeTransform();

	Vector3 moveDelta = Vector3::Zero;

	if (Keyboard::Get()->Press('W'))
		moveDelta += T->GetForwardVector() * MoveSpeed * deltaTime;

	if (Keyboard::Get()->Press('S'))
		moveDelta -= T->GetForwardVector() * MoveSpeed * deltaTime;

	if (Keyboard::Get()->Press('D'))
		moveDelta += T->GetRightVector() * MoveSpeed * deltaTime;

	if (Keyboard::Get()->Press('A'))
		moveDelta -= T->GetRightVector() * MoveSpeed * deltaTime;

	if (moveDelta.LengthSquared() > 0.0f) // 이동량이 있으면
	{
		Vector3 pos = T->GetPosition();
		T->SetPosition(pos + moveDelta);
	}

	Vector3 mouseDelta = Mouse::Get()->GetMoveDelta();


	float pitch = mouseDelta.y * RotationSpeed * deltaTime;
	float yaw = mouseDelta.x * RotationSpeed * deltaTime;

	T->AddRotation(yaw, pitch, 0.0f);

	SetViewMatrix();


	string str = string("FrameRate : ") + to_string((int)ImGui::GetIO().Framerate);
	Gui::Get()->RenderText(5, 5, 1, 1, 1, str);

	str = String::Format("Camera Rotation : %3.0f, %3.0f, %3.0f", T->GetRotation().x, T->GetRotation().y, T->GetRotation().z);
	Gui::Get()->RenderText(5, 20, 1, 1, 1, str);

	str = String::Format("Camera Position : %3.0f, %3.0f, %3.0f", T->GetPosition().x, T->GetPosition().y, T->GetPosition().z);
	Gui::Get()->RenderText(5, 35, 1, 1, 1, str);

	str = String::Format("MoveDelta : %f, %f, %f", moveDelta.x, moveDelta.y, moveDelta.z);
	Gui::Get()->RenderText(5, 50, 1, 1, 1, str);
}



void UCameraComponent::SetViewMatrix()
{
	FTransform* T = GetWorldTransform();
    
	// 카메라의 위치와 방향 설정
	Vector3 position = T->GetPosition();
	Vector3 target = position + T->GetForwardVector();
	Vector3 up = T->GetUpVector();

	if (up.Length() <= 0.f)
	{
		up = Vector3(0, 1, 0);
		target = Vector3(0, 0, 1);
	}
	// View 행렬 생성
	aspect = D3D::Get()->GetDesc().Width / D3D::Get()->GetDesc().Height;
	ViewContext.Projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspect, 0.1f, 1000.0f);
	ViewContext.View = XMMatrixLookAtLH(position, target, up);
	ViewContext.ViewInverse = ViewContext.View.Invert();
	ViewContext.ViewProjection = ViewContext.View * ViewContext.Projection;
	ViewContext.EyePos = position;

	
	FSceneView::Get()->UpdateSceneView(ViewContext);
}




