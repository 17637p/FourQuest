#include "DecalCommon.hlsli"

VertexOut main(float3 position : POSITION, float2 UV : UV)
{
    VertexOut vout;
    
    vout.PosH = mul(float4(position, 1), gWorldMatrix);
    vout.PosH = mul(vout.PosH, gViewMatrix);
    vout.PosH = mul(vout.PosH, gProjMatrix);
    
    vout.PosClip = mul(float4(position, 1), gWorldMatrix);
    vout.PosClip = mul(vout.PosClip, gViewMatrix);
    vout.PosClip = mul(vout.PosClip, gProjMatrix);
    
    vout.UV = mul(float4(UV, 0, 1), gTexMatrix);
    
    vout.Orientation = normalize(float3(gWorldMatrix[1][0], gWorldMatrix[1][1], gWorldMatrix[1][2]));
    
    return vout;
}