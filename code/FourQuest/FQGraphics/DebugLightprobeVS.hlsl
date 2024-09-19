
struct VertexIn
{
    float3 Position : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 UV : UV;
};

struct VertexOut
{
	float4 PositionH : SV_POSITION;
    float3 NormalW : NORMAL;
};

cbuffer cbModelTransform : register(b0)
{
    float4x4 cWorld;
    float4x4 cWorldInvTranspose;
};

cbuffer cbSceneTransform : register(b1)
{
    float4x4 cView;
    float4x4 cViewProj;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    float4x4 worldMat;
    
    worldMat = cWorld;
    
    vout.PositionH = mul(float4(vin.Position, 1.f), worldMat);
    vout.PositionH = mul(vout.PositionH, cViewProj);
    
    vout.NormalW = normalize(mul(vin.NormalL, (float3x3) worldMat));
    
    return vout;
}