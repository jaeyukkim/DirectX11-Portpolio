#pragma once
#include "HeaderCollection.h"

struct FTransform
{
public:
    Vector3 Translation; // 위치
    Quaternion Rotation;    // 쿼터니언
    Vector3 Scale;       // 스케일



    FTransform()
        : Translation(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
    {
    }

    FTransform(XMVECTOR translation, XMVECTOR rotation, XMVECTOR scale)
        : Translation(translation), Rotation(rotation), Scale(scale)
    {
    }
    

    // 변환을 행렬로 변환
    Matrix ToMatrix() const
    {
        Matrix S = Matrix::CreateScale(Scale);
        Matrix R = Matrix::CreateFromQuaternion(Rotation);
        Matrix T = Matrix::CreateTranslation(Translation);
        return S * R * T;
    }

    // 위치 설정
    void SetTranslation(float x, float y, float z)
    {
        Translation = XMVectorSet(x, y, z, 0);
    }
    void SetTranslation(Vector3 InTranslation)
    {
        Translation = InTranslation;
    }

    // 회전 설정 (Euler → Quaternion)
    void SetRotationFromEuler(float pitch, float yaw, float roll)
    {
        Rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    }
    void SetRotationFromEuler(Quaternion InRotation)
    {
        Rotation = InRotation;
    }

    // 스케일 설정
    void SetScale(float x, float y, float z)
    {
        Scale = XMVectorSet(x, y, z, 0);
    }
    void SetScale(Vector3 InScale)
    {
        Scale = InScale;
    }

    // 위치 벡터에 변환 적용
    Vector4 operator*(const Vector4& vec) const
    {
        // 스케일 → 회전 → 위치 적용
        Vector4 scaled = XMVectorMultiply(vec, Scale);                       // vec * scale
        Quaternion rotated = XMVector3Rotate(scaled, Rotation);                // 회전 (Quaternion)
        Vector4 translated = XMVectorAdd(rotated, Translation);             // 위치 추가
        return translated;
    }
};