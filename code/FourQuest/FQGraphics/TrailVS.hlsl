#include "Common.hlsli"

struct VertexIn
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 UV : UV;
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
};

cbuffer perFrame : register(b0)
{
    float4x4 gViewProj;
}

cbuffer cbMaterial : register(b1)
{
    ParticleMaterial gParticleMaterial;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    vout.PositionH = mul(float4(vin.Position, 1.f), gViewProj);
    vout.Color = vin.Color;
    vout.UV = mul(float4(vin.UV, 0, 1), gParticleMaterial.TexTransform);
    
    return vout;
}