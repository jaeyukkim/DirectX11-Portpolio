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

