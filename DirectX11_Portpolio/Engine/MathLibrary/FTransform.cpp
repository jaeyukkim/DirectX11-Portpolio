#include "HeaderCollection.h"
#include "FTransform.h"

FTransform::FTransform()
    : Position(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
{
}

FTransform::FTransform(XMVECTOR Position, XMVECTOR rotation, XMVECTOR scale)
    : Position(Position), Rotation(rotation), Scale(scale)
{
}

Matrix FTransform::ToMatrix() const
{
    Matrix S = Matrix::CreateScale(Scale);
    Matrix R = Matrix::CreateFromQuaternion(Rotation);
    Matrix T = Matrix::CreateTranslation(Position);
    return S * R * T;
}

void FTransform::SetTransform(const FTransform& InTransform)
{
    *this = InTransform;
}

void FTransform::SetTransformFromMatrix(const Matrix& M)
{
    XMVECTOR outScale, outRotationQuat, outTranslation;

    if (XMMatrixDecompose(&outScale, &outRotationQuat, &outTranslation, M))
    {
        Scale = outScale;
        Rotation = outRotationQuat;
        Position = outTranslation;
    }
    else
    {
        Scale = XMVectorSet(1, 1, 1, 0);
        Rotation = XMQuaternionIdentity();
        Position = XMVectorZero();
    }
}

Vector3 FTransform::GetPosition() const { return Position; }
Vector3 FTransform::GetScale() const { return Scale; }

Vector3 FTransform::GetRotation() const
{
    XMMATRIX rotMat = XMMatrixRotationQuaternion(Rotation);

    float pitch = asinf(-rotMat.r[2].m128_f32[1]);

    float yaw, roll;
    if (fabs(cosf(pitch)) > 0.0001f)
    {
        yaw = atan2f(rotMat.r[2].m128_f32[0], rotMat.r[2].m128_f32[2]);
        roll = atan2f(rotMat.r[0].m128_f32[1], rotMat.r[1].m128_f32[1]);
    }
    else
    {
        yaw = atan2f(-rotMat.r[1].m128_f32[0], rotMat.r[0].m128_f32[0]);
        roll = 0.0f;
    }

    pitch = XMConvertToDegrees(pitch);
    yaw = XMConvertToDegrees(yaw);
    roll = XMConvertToDegrees(roll);

    pitch = NormalizeAngle(pitch);
    yaw = NormalizeAngle(yaw);
    roll = NormalizeAngle(roll);

    return Vector3(pitch, yaw, roll);
}

void FTransform::SetPosition(float x, float y, float z)
{
    Position = XMVectorSet(x, y, z, 0);
}

void FTransform::SetPosition(Vector3 InPosition)
{
    Position = InPosition;
}

void FTransform::SetRotation(float Inpitch, float Inyaw, float Inroll)
{
    Rotation = XMQuaternionRotationRollPitchYaw(
        XMConvertToRadians(NormalizeAngle(Inpitch)),
        XMConvertToRadians(NormalizeAngle(Inyaw)),
        XMConvertToRadians(NormalizeAngle(Inroll))
    );
}

void FTransform::SetRotation(Vector3 InRotation)
{
    InRotation.x = NormalizeAngle(InRotation.x);
    InRotation.y = NormalizeAngle(InRotation.y);
    InRotation.z = NormalizeAngle(InRotation.z);

    Rotation = XMQuaternionRotationRollPitchYaw(
        XMConvertToRadians(InRotation.x),
        XMConvertToRadians(InRotation.y),
        XMConvertToRadians(InRotation.z)
    );
}

void FTransform::SetScale(float x, float y, float z)
{
    Scale = XMVectorSet(x, y, z, 0);
}

void FTransform::SetScale(Vector3 InScale)
{
    Scale = InScale;
}

Vector4 FTransform::operator*(const Vector4& vec) const
{
    Vector4 scaled = XMVectorMultiply(vec, Scale);
    Quaternion rotated = XMVector3Rotate(scaled, Rotation);
    Vector4 translated = XMVectorAdd(rotated, Position);
    return translated;
}

Vector3 FTransform::GetForwardVector() const
{
    return XMVector3Normalize(XMVector3Rotate(Vector3::Forward, Rotation));
}

Vector3 FTransform::GetRightVector() const
{
    return XMVector3Normalize(XMVector3Rotate(Vector3::Right, Rotation));
}

Vector3 FTransform::GetUpVector() const
{
    return XMVector3Normalize(XMVector3Rotate(Vector3::Up, Rotation));
}

void FTransform::SetForwardVector(Vector3 InForward, Vector3 Up)
{
    Rotation = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), InForward, Up));
}

void FTransform::SetRightVector(Vector3 InRight, Vector3 Up)
{
    Vector3 Forward = XMVector3Cross(Up, InRight);
    Rotation = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), Forward, Up));
}

void FTransform::SetUpVector(Vector3 InUp, Vector3 Forward)
{
    Rotation = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), Forward, InUp));
}

void FTransform::AddRotation(float yawDelta, float pitchDelta, float rollDelta)
{
    Vector3 rot = GetRotation();
    SetRotation(rot.x + pitchDelta, rot.y + yawDelta, rot.z + rollDelta);
}

float FTransform::NormalizeAngle(float angle)
{
    angle = fmodf(angle, 360.0f);
    if (angle > 180.0f)
        angle -= 360.0f;
    else if (angle < -180.0f)
        angle += 360.0f;

    return angle;
}
