#define MATERIAL_TEXTURE_DIFFUSE 0
#define MATERIAL_TEXTURE_SPECULAR 1
#define MATERIAL_TEXTURE_NORMAL 2
#define MAX_MATERIAL_TEXTURE_COUNT 3

Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT];


struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;

    float2 Tiling;
};