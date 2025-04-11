#pragma once
#include "HeaderCollection.h"

struct FTransform
{
    XMVECTOR Translation; // 위치
    XMVECTOR Rotation;    // 쿼터니언
    XMVECTOR Scale;       // 스케일

    FTransform()
        : Translation(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
    {
    }

    FTransform(XMVECTOR translation, XMVECTOR rotation, XMVECTOR scale)
        : Translation(translation), Rotation(rotation), Scale(scale)
    {
    }

    // 변환을 행렬로 변환
    XMMATRIX ToMatrix() const
    {
        XMMATRIX S = XMMatrixScalingFromVector(Scale);
        XMMATRIX R = XMMatrixRotationQuaternion(Rotation);
        XMMATRIX T = XMMatrixTranslationFromVector(Translation);
        return S * R * T;
    }

    // 위치 설정
    void SetTranslation(float x, float y, float z)
    {
        Translation = XMVectorSet(x, y, z, 0);
    }

    // 회전 설정 (Euler → Quaternion)
    void SetRotationFromEuler(float pitch, float yaw, float roll)
    {
        Rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    }

    // 스케일 설정
    void SetScale(float x, float y, float z)
    {
        Scale = XMVectorSet(x, y, z, 0);
    }

    // 위치 벡터에 변환 적용
    XMVECTOR operator*(const XMVECTOR& vec) const
    {
        // 스케일 → 회전 → 위치 적용
        XMVECTOR scaled = XMVectorMultiply(vec, Scale);                       // vec * scale
        XMVECTOR rotated = XMVector3Rotate(scaled, Rotation);                // 회전 (Quaternion)
        XMVECTOR translated = XMVectorAdd(rotated, Translation);             // 위치 추가
        return translated;
    }
};