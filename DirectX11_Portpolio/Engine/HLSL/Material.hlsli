

#define MATERIAL_TEXTURE_Diffuse 0
#define MATERIAL_TEXTURE_Specular 1
#define MATERIAL_TEXTURE_NORMAL 2
#define MAX_MATERIAL_TEXTURE_COUNT 3

Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT];

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material : register(b0)
{
    MaterialDesc Material;
};