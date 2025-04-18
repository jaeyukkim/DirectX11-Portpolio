#pragma once

struct FViewContext
{
    Matrix View = Matrix::Identity;
    Matrix ViewInverse = Matrix::Identity;
    Matrix Projection = Matrix::Identity;
    Matrix ViewProjection = Matrix::Identity;
};

struct FSceneView
{
public:
    static FSceneView* Get();
    static void Create();
    static void Destroy();
    static void PreRender();

    void UpdateSceneView(const Matrix& Inview, const Matrix& Inprojection);
    D3D_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return Instance->IAPrimitive; }
    FViewContext* GetSceneViewContext() { Context; }

public:
    FSceneView() = default;
    ~FSceneView() = default;

private:
  
    FViewContext Context;
    shared_ptr<ConstantBuffer> ViewConstantBuffer;


    static FSceneView* Instance;

    D3D_PRIMITIVE_TOPOLOGY IAPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
