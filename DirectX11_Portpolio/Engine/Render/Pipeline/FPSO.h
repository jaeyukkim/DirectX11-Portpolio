#pragma once

struct FPSO
{
public:
    void operator=(const FPSO &pso);
    void SetBlendFactor(const array<float, 4> InBlendFactor);

public:
    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
    ComPtr<ID3D11HullShader> m_hullShader = nullptr;
    ComPtr<ID3D11DomainShader> m_domainShader = nullptr;
    ComPtr<ID3D11GeometryShader> m_geometryShader = nullptr;
    ComPtr<ID3D11InputLayout> m_inputLayout = nullptr;

    ComPtr<ID3D11BlendState> m_blendState = nullptr;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState = nullptr;
    ComPtr<ID3D11RasterizerState> m_rasterizerState = nullptr;
    
    array<float, 4> m_blendFactor = { 0.0f, 0.0f, 0.0f, 1.0f };
    UINT m_stencilRef = 0;

    D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology =
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
