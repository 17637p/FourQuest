struct VertexIn
{
    float3 Position : POSITION;
    float2 UV : UV;
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

cbuffer perFrame : register(b0)
{
    float4x4 gViewProj;
}

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    vout.PositionH = mul(float4(vin.Position, 1.f), gViewProj);
    vout.UV = vin.UV;
    return vout;
}