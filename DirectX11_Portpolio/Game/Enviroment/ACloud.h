#pragma once
#include "Frameworks/Objects/Actor.h"


class UBoxComponent;

struct VolumeConsts
{
    Vector3 uvwOffset = Vector3(0.0f);
    float lightAbsorption = 5.0f;
    Vector3 lightDir = Vector3(0.0f, 1.0f, 0.0f);
    float densityAbsorption = 10.0f;
    Vector3 lightColor = Vector3(1, 1, 1) * 40.0f;
    float aniso = 0.3f;
};


class ACloud : public Actor
{
public:
    ACloud();

    virtual void InitAllComponents() override;
    virtual void UpdateGUI();
    virtual void Tick(float deltaTime) override;
    virtual void CustomRender(float deltaTime) override;

protected:
    int m_volumeWidth = 128;
    int m_volumeHeight = 128;
    int m_volumeDepth = 128;
    int m_lightWidth = 128 / 4; // 라이트맵은 낮은 해상도
    int m_lightHeight = 128 / 4;
    int m_lightDepth = 128 / 4;
    float Offset = 0.0f;
    
    shared_ptr<UBoxComponent> boxCollision;
    StaticMeshData meshData;
    VolumeConsts m_volumeConstsCpu;

    Matrix meshWorld;
    FTransform meshTransform;
    shared_ptr<ConstantBuffer> WorldCBuffer;
    
private:
    ComPtr<ID3D11ComputeShader> m_cloudDensityCS;
    ComPtr<ID3D11ComputeShader> m_cloudLightingCS;
    
    shared_ptr<ConstantBuffer> m_volumeConstsGpu;
    shared_ptr<IndexBuffer> IBuffer;
    shared_ptr<VertexBuffer> VBuffer;
    Texture3D densityTex;
    Texture3D lightingTex;

private:
    const wstring HlslPath = L"../../Engine/Render/HLSL/Cloud/";
    const wstring CloudDensityCSPath = HlslPath + L"CloudDensityCS.hlsl";
    const wstring CloudLightingCSPath = HlslPath + L"CloudLightingCS.hlsl";
};
