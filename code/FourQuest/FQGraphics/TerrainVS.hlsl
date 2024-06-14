struct VertexIn
{
    float3 Position : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 UV : TEXCOORD0;
    float2 BoundsY : TEXCOORD1;
};

struct VertexOut
{
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
    float2 BoundsY : TEXCOORD1;
};

cbuffer cbModelTransform : register(b0)
{
    float4x4 cWorld;
    float4x4 cWorldInvTranspose;
};

Texture2D gHeightMap : register(t0);
SamplerState samHeightmap : register(s0); 

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    float4x4 worldMat;

    worldMat = cWorld;
    
    // Hull Shader에서 거리를 재야하므로 월드를 곱해줘야 함
    vout.PositionW = vin.Position;
    vout.PositionW.y = gHeightMap.SampleLevel(samHeightmap, vin.UV, 0).r;
    vout.PositionW = mul(float4(vin.Position, 1.f), worldMat);

    vout.NormalW = normalize(mul(vin.NormalL, (float3x3) cWorldInvTranspose));
    vout.TangentW = normalize(mul(vin.TangentL, (float3x3) worldMat));
    vout.UV = vin.UV;
    vout.BoundsY = vin.BoundsY;   

    return vout;
}