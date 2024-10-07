#include "DecalCommon.hlsli"


static const matrix gTexTransform =
{
    0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f
};

VertexOut main(float3 position : POSITION, float2 UV : UV)
{
    VertexOut vout;
    
    vout.PosH = mul(float4(position, 1), gWorldMatrix);
    vout.PosH = mul(vout.PosH, gViewMatrix);
    vout.PosH = mul(vout.PosH, gProjMatrix);
    vout.PosTemp = mul(vout.PosH, gTexTransform);
    
    vout.UV = mul(float4(UV, 0, 1), gTexMatrix);
    
    vout.Orientation = normalize(float3(gWorldMatrix[1][0], gWorldMatrix[1][1], gWorldMatrix[1][2]));
    
    return vout;
}