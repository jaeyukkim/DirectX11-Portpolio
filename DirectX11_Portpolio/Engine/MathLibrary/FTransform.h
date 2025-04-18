#pragma once
#include "HeaderCollection.h"



struct FTransform
{
private:
    Vector3 Position; // 위치
    Quaternion Rotation;    // 쿼터니언
    Vector3 Scale;       // 스케일


public:
    FTransform()
        : Position(XMVectorZero()), Rotation(XMQuaternionIdentity()), Scale(XMVectorSet(1, 1, 1, 0))
    {
    }

    FTransform(XMVECTOR Position, XMVECTOR rotation, XMVECTOR scale)
        : Position(Position), Rotation(rotation), Scale(scale)
    {
    }
    

    // 변환을 행렬로 변환
    Matrix ToMatrix() const
    {
        Matrix S = Matrix::CreateScale(Scale);
        Matrix R = Matrix::CreateFromQuaternion(Rotation);
        Matrix T = Matrix::CreateTranslation(Position);
        return S * R * T;
    }

    inline Vector3 GetPosition() const { return Position; }
    inline Quaternion GetRotation() const { return Rotation; }    // 쿼터니언
    inline Vector3 GetScale() const { return Scale; }

    // 위치 설정
    void SetPosition(float x, float y, float z)
    {
        Position = XMVectorSet(x, y, z, 0);
    }
    void SetPosition(Vector3 InPosition)
    {
        Position = InPosition;
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
        Vector4 translated = XMVectorAdd(rotated, Position);             // 위치 추가
        return translated;
    }

    Vector3 GetForwardVector() const
    {
        return XMVector3Rotate(Vector3::Forward, Rotation); // (0, 0, 1)
    }

    Vector3 GetRightVector() const
    {
        return XMVector3Rotate(Vector3::Right, Rotation);   // (1, 0, 0)
    }

    Vector3 GetUpVector() const
    {
        return XMVector3Rotate(Vector3::Up, Rotation);      // (0, 1, 0)
    }

    // ======= 방향 벡터 설정 (회전을 방향 벡터로 설정) =======

    void SetForwardVector(Vector3 InForward, Vector3 Up = Vector3::Up)
    {
        Rotation = XMQuaternionRotationMatrix(XMMatrixLookToRH(XMVectorZero(), InForward, Up));
    }

    void SetRightVector(Vector3 InRight, Vector3 Up = Vector3::Up)
    {
        Vector3 Forward = XMVector3Cross(Up, InRight);
        Rotation = XMQuaternionRotationMatrix(XMMatrixLookToRH(XMVectorZero(), Forward, Up));
    }

    void SetUpVector(Vector3 InUp, Vector3 Forward = Vector3::Forward)
    {
        Rotation = XMQuaternionRotationMatrix(XMMatrixLookToRH(XMVectorZero(), Forward, InUp));
    }


    void FTransform::AddRotation(float yawDelta, float pitchDelta, float rollDelta)
    {
        // Δ값들을 Quaternion으로 변환 (Yaw → Y축, Pitch → X축, Roll → Z축 기준)
        Quaternion qPitch = XMQuaternionRotationAxis(Vector3::Right, pitchDelta);
        Quaternion qYaw = XMQuaternionRotationAxis(Vector3::Up, yawDelta);
        Quaternion qRoll = XMQuaternionRotationAxis(Vector3::Forward, rollDelta);

        // 회전 순서: Roll → Pitch → Yaw (보통은 이 순서, 필요에 따라 바꿔도 됨)
        Quaternion deltaRotation = XMQuaternionMultiply(qRoll, XMQuaternionMultiply(qPitch, qYaw));

        // 기존 회전에 곱해줌 (주의: 순서에 따라 결과가 다름)
        Rotation = XMQuaternionNormalize(XMQuaternionMultiply(deltaRotation, Rotation));
    }

};