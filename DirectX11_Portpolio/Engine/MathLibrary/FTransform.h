#pragma once
#include "HeaderCollection.h"

struct FTransform
{
    XMVECTOR Translation; // ��ġ
    XMVECTOR Rotation;    // ���ʹϾ�
    XMVECTOR Scale;       // ������

    FTransform()
        : Translation(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
    {
    }

    FTransform(XMVECTOR translation, XMVECTOR rotation, XMVECTOR scale)
        : Translation(translation), Rotation(rotation), Scale(scale)
    {
    }

    // ��ȯ�� ��ķ� ��ȯ
    XMMATRIX ToMatrix() const
    {
        XMMATRIX S = XMMatrixScalingFromVector(Scale);
        XMMATRIX R = XMMatrixRotationQuaternion(Rotation);
        XMMATRIX T = XMMatrixTranslationFromVector(Translation);
        return S * R * T;
    }

    // ��ġ ����
    void SetTranslation(float x, float y, float z)
    {
        Translation = XMVectorSet(x, y, z, 0);
    }

    // ȸ�� ���� (Euler �� Quaternion)
    void SetRotationFromEuler(float pitch, float yaw, float roll)
    {
        Rotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    }

    // ������ ����
    void SetScale(float x, float y, float z)
    {
        Scale = XMVectorSet(x, y, z, 0);
    }

    // ��ġ ���Ϳ� ��ȯ ����
    XMVECTOR operator*(const XMVECTOR& vec) const
    {
        // ������ �� ȸ�� �� ��ġ ����
        XMVECTOR scaled = XMVectorMultiply(vec, Scale);                       // vec * scale
        XMVECTOR rotated = XMVector3Rotate(scaled, Rotation);                // ȸ�� (Quaternion)
        XMVECTOR translated = XMVectorAdd(rotated, Translation);             // ��ġ �߰�
        return translated;
    }
};