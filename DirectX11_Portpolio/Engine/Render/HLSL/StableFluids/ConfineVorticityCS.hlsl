Texture2D<float> vorticity : register(t0);
RWTexture2D<float2> velocity : register(u0);

cbuffer Consts : register(b0)
{
    float dt;
    float viscosity;
    float2 sourcingVelocity;
    float4 sourcingDensity;
    uint i;
    uint j;
}

[numthreads(32, 32, 1)]
void CS_Main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
    uint3 dtID : SV_DispatchThreadID)
{
    uint width, height;
    velocity.GetDimensions(width, height);
    float2 dx = float2(1.0 / width, 1.0 / height);

    // Vorticity confinement
    uint2 left = uint2(dtID.x == 0 ? width - 1 : dtID.x - 1, dtID.y);
    uint2 right = uint2(dtID.x == width - 1 ? 0 : dtID.x + 1, dtID.y);
    uint2 up = uint2(dtID.x, dtID.y == height - 1 ? 0 : dtID.y + 1);
    uint2 down = uint2(dtID.x, dtID.y == 0 ? height - 1 : dtID.y - 1);

    float2 eta = (abs(velocity[right].y) - abs(velocity[left].y)) / (2.0 * dx.x) -
        (abs(velocity[up].x) - abs(velocity[down].x)) / (2.0 * dx.y);

    if (length(eta) < 1e-5)
        return;

    float3 psi = float3(normalize(eta), 0.f);
    float3 omega = float3(0, 0, vorticity[dtID.xy]);

    velocity[dtID.xy] += 0.2 * cross(psi, omega).xy * dx;
}
