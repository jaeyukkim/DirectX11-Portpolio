#pragma once
#include "HeaderCollection.h"

struct FTransform
{
public:
    Vector3 Translation; // ��ġ
    Quaternion Rotation;    // ���ʹϾ�
    Vector3 Scale;       // ������



    FTransform()
        : Translation(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
    {
    }

    FTransform(XMVECTOR translation, XMVECTOR rotation, XMVECTOR scale)
        : Translation(translation), Rotation(rotation), Scale(scale)
    {
    }
    

    // ��ȯ�� ��ķ� ��ȯ
    Matrix ToMatrix() const
    {
        Matrix S = Matrix::CreateScale(Scale);
        Matrix R = Matrix::CreateFromQuaternion(Rotation);
        Matrix T = Matrix::CreateTranslation(Translation);
        return S * R * T;
    }

    // ��ġ ����
    void SetTranslation(float x, float y, float z)
    {
        Translation = XMVectorSet(x, y, z, 0);
    }
    void SetTranslation(Vector3 InTranslation)
    {
        Translation = InTranslation;
    }

    // ȸ�� ���� (Euler �� Quaternion)
    void SetRotationFromEuler(float pitch, float yaw, float roll)
    {
        Rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
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
        Vector4 translated = XMVectorAdd(rotated, Translation);             // ��ġ �߰�
        return translated;
    }
};