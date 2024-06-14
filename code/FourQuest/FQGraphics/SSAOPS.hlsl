#include "Common.hlsli"

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionV : POSITION;
    float3 NormalV : NORMAL;
};

float4 main(VertexOut pin) : SV_TARGET
{
    pin.NormalV = normalize(pin.NormalV);

    return float4(pin.NormalV, pin.PositionV.z);
}