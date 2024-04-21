
struct VertexIn
{
    float3 Position : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 UV : UV;
#ifdef SKINNING
    int4 Indices : INDICES;
    float4 Weights : WEIGHTS;
#elif defined INSTANCING
    float4x4 World : WORLD;
#endif
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float4 PositionShadowTex : TEXCOORD2;
};

cbuffer cbModelTransform : register(b0)
{
    float4x4 cWorld;
    float4x4 cWorldInvTranspose;
};

cbuffer cbSceneTransform : register(b1)
{
    float4x4 cViewProj;
    float4x4 cShadowViewProjTex;
};

#ifdef SKINNING
cbuffer cbBonePalette : register(b2)
{
    float4x4 cBonePalette[128];
};
#endif

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    float4x4 worldMat;
    
#ifdef SKINNING 
    worldMat = mul(vin.Weights.x, cBonePalette[vin.Indices.x]);
    worldMat += mul(vin.Weights.y, cBonePalette[vin.Indices.y]);
    worldMat += mul(vin.Weights.z, cBonePalette[vin.Indices.z]);
    worldMat += mul(vin.Weights.w, cBonePalette[vin.Indices.w]);
   // worldMat = mul(worldMat, cWorld);
#elif defined INSTANCING
    worldMat = vin.World;
#else
    worldMat = cWorld;
#endif 
    
    vout.PositionH = mul(float4(vin.Position, 1.f), worldMat);
    vout.PositionH = mul(vout.PositionH, cViewProj);
    
    vout.PositionW = mul(float4(vin.Position, 1.f), worldMat);

    vout.NormalW = normalize(mul(vin.NormalL, (float3x3) cWorldInvTranspose));
    
    vout.TangentW = normalize(mul(vin.TangentL, (float3x3) cWorldInvTranspose));
    
    vout.UV = vin.UV;
    
    vout.PositionShadowTex = mul(float4(vin.Position, 1.f), worldMat);
    vout.PositionShadowTex = mul(vout.PositionShadowTex, cShadowViewProjTex);
    
    return vout;
}