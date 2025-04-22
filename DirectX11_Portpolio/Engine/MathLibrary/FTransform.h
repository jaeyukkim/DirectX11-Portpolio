#pragma once
#include "HeaderCollection.h"



struct FTransform
{
public:
    Vector3 Position; // ��ġ
    Quaternion Rotation;    // ���ʹϾ�
    Vector3 Scale;       // ������

    float Yaw = 0.0f;
    float Pitch = 0.0f;

public:
    FTransform()
        : Position(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
    {
    }

    FTransform(XMVECTOR Position, XMVECTOR rotation, XMVECTOR scale)
        : Position(Position), Rotation(rotation), Scale(scale)
    {
    }
    

    // ��ȯ�� ��ķ� ��ȯ
    Matrix ToMatrix() const
    {
        Matrix S = Matrix::CreateScale(Scale);
        Matrix R = Matrix::CreateFromQuaternion(Rotation);
        Matrix T = Matrix::CreateTranslation(Position);
        return S * R * T;
    }

    inline Vector3 GetPosition() const { return Position; }
    inline Quaternion GetRotation() const { return Rotation; }    // ���ʹϾ�
    inline Vector3 GetScale() const { return Scale; }

    // ��ġ ����
    void SetPosition(float x, float y, float z)
    {
        Position = XMVectorSet(x, y, z, 0);
    }
    void SetPosition(Vector3 InPosition)
    {
        Position = InPosition;
    }

    // ȸ�� ���� (Euler �� Quaternion)
    void SetRotationFromEuler(float Inpitch, float Inyaw, float Inroll)
    {
      
        Rotation = XMVector3Normalize(XMQuaternionRotationRollPitchYaw(Inpitch, Inyaw, Inroll));
    }
    void SetRotationFromEuler(Quaternion InRotation)
    {
        Rotation = InRotation;
    }

    // ������ ����
    void SetScale(float x, float y, float z)
    {
        Scale = XMVectorSet(x, y, z, 0);
    }
    void SetScale(Vector3 InScale)
    {
        Scale = InScale;
    }


    // ��ġ ���Ϳ� ��ȯ ����
    Vector4 operator*(const Vector4& vec) const
    {
        // ������ �� ȸ�� �� ��ġ ����
        Vector4 scaled = XMVectorMultiply(vec, Scale);                       // vec * scale
        Quaternion rotated = XMVector3Rotate(scaled, Rotation);                // ȸ�� (Quaternion)
        Vector4 translated = XMVectorAdd(rotated, Position);             // ��ġ �߰�
        return translated;
    }

    Vector3 GetForwardVector() const
    {
        return XMVector3Normalize(XMVector3Rotate(Vector3::Forward, Rotation)); // (0, 0, 1)
    }

    Vector3 GetRightVector() const
    {
        return XMVector3Normalize(XMVector3Rotate(Vector3::Right, Rotation));   // (1, 0, 0)
    }

    Vector3 GetUpVector() const
    {
        return XMVector3Normalize(XMVector3Rotate(Vector3::Up, Rotation));      // (0, 1, 0)
    }

    // ======= ���� ���� ���� (ȸ���� ���� ���ͷ� ����) =======

    void SetForwardVector(Vector3 InForward, Vector3 Up = Vector3::Up)
    {
        Rotation = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), InForward, Up));
    }

    void SetRightVector(Vector3 InRight, Vector3 Up = Vector3::Up)
    {
        Vector3 Forward = XMVector3Cross(Up, InRight);
        Rotation = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), Forward, Up));
    }

    void SetUpVector(Vector3 InUp, Vector3 Forward = Vector3::Forward)
    {
        Rotation = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMVectorZero(), Forward, InUp));
    }


    void FTransform::AddRotation(float yawDelta, float pitchDelta, float rollDelta)
    {
        Yaw += yawDelta;
        Pitch += pitchDelta;

        // Clamp pitch to avoid flipping
        Pitch = std::clamp(Pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);


        SetRotationFromEuler(Pitch, Yaw, 0.0f);
        
    }

};