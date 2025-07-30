#pragma once
#include "Libraries/Utility/Delegate.h"



struct FTransform
{
public:
    Vector3 Position;
    Quaternion Rotation;
    Vector3 Scale;

    // 제한 각도 (도 단위)
    Vector3 MinRotation = Vector3(-89.0f, -360.0f, -180.0f);
    Vector3 MaxRotation = Vector3(89.0f, 360.0f, 180.0f);

    // 축 잠금 (true면 해당 축은 회전하지 않음)
    bool bLockPitch = false;
    bool bLockYaw   = false;
    bool bLockRoll  = true;
    
public:
    FTransform();
    FTransform(XMVECTOR Position, XMVECTOR rotation, XMVECTOR scale);

    Matrix ToMatrix() const;
    void SetTransform(const FTransform& InTransform);
    void SetTransformFromMatrix(const Matrix& M);

    Vector3 GetPosition() const;
    Vector3 GetScale() const;
    Vector3 GetRotation() const;
    Quaternion GetQuat() const;

    void SetPosition(float x, float y, float z);
    void SetPosition(Vector3 InPosition);

    void SetRotation(float Inpitch, float Inyaw, float Inroll);
    void SetRotation(Vector3 InRotation);

    void SetScale(float x, float y, float z);
    void SetScale(Vector3 InScale);

    void SetQuat(Quaternion InQuat); 

    Vector4 operator*(const Vector4& vec) const;
    Vector4 operator=(const Vector4& vec) const;


    Vector3 GetForwardVector() const;
    Vector3 GetRightVector() const;
    Vector3 GetUpVector() const;

    void SetForwardVector(Vector3 InForward, Vector3 Up = Vector3::Up);
    void SetRightVector(Vector3 InRight, Vector3 Up = Vector3::Up);
    void SetUpVector(Vector3 InUp, Vector3 Forward = Vector3::Forward);

    void AddRotation(float yawDelta, float pitchDelta, float rollDelta);
    
    void AddPitchInput(float InVal);
    void AddYawInput(float InVal);
    void AddRollInput(float InVal);

private:
    static float NormalizeAngle(float angle);
};