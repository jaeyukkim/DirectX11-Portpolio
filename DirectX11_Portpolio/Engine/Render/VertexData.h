#pragma once

//------------------------------------------------------------------------------

struct Vertex
{
    Vertex()
        : Position(0, 0, 0) {}

    Vector3 Position;
};

//------------------------------------------------------------------------------

struct VertexColor
{
    VertexColor()
        : Position(0, 0, 0)
        , Color(0, 0, 0, 1) {}

    VertexColor(XMFLOAT3 p, XMFLOAT4 c)
        : Position(p)
        , Color(c) {}

    Vector3 Position;
    Color Color;
};

//------------------------------------------------------------------------------

struct VertexTexture
{
    VertexTexture()
        : Position(0, 0, 0)
        , Uv(0, 0) {}

    VertexTexture(XMFLOAT3 p, XMFLOAT2 c)
        : Position(p)
        , Uv(c) {}

    Vector3 Position;
    Vector2 Uv;
};

//------------------------------------------------------------------------------

struct VertexTextureColor
{
    VertexTextureColor()
        : Position(0, 0, 0)
        , Uv(0, 0)
        , Color(0, 0, 0, 1) {}

    Vector3 Position;
    Vector2 Uv;
    Color Color;
};

//------------------------------------------------------------------------------

struct VertexNormal
{
    VertexNormal()
        : Position(0, 0, 0)
        , Normal(0, 0, 0) {}

    Vector3 Position;
    Vector3 Normal;
};

//------------------------------------------------------------------------------

struct VertexModel
{
    Vector3 Position;
    Vector2 Uv;
    Color Color;
    Vector3 Normal;
    Vector3 Tangent;
    Vector4 Indices; //int4
    Vector4 Weights; //float4
};

struct VertexObject
{
    Vector3 Position;
    Vector2 Uv;
    Color Color;
    Vector3 Normal;
    Vector3 Tangent;
};

class InputElementCollection
{
public:

    inline static vector<D3D11_INPUT_ELEMENT_DESC> basicInputElement =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    inline static vector<D3D11_INPUT_ELEMENT_DESC> SkeletalMeshInputElement =
    {
        { "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
        { "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 8 bytes
        { "COLOR",        0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
        { "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
        { "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 12 bytes
        { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 60,  D3D11_INPUT_PER_VERTEX_DATA, 0 },   // 16 bytes
        { "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 76,  D3D11_INPUT_PER_VERTEX_DATA, 0 }   // 16 bytes
    };
};


class SamplerDescCollection
{
public:
    static D3D11_SAMPLER_DESC GetDefaultSamplerDesc()
    {
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        return desc;
    }
};