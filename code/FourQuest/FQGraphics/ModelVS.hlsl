
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
    float ClipSpacePosZ : TEXCOORD2;
    float DepthView : TEXCOORD3;
    float3 NormalV : TEXCOORD4;
    float3 TangentV : TEXCOORD5;
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

#ifdef SKINNING
cbuffer cbBoneTransform : register(b2)
{
    float4x4 cFinalTransforms[128];
};
#endif

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    float4x4 worldMat;
    
#ifdef SKINNING 
    worldMat = mul(vin.Weights.x, cFinalTransforms[vin.Indices.x]);
    worldMat += mul(vin.Weights.y, cFinalTransforms[vin.Indices.y]);
    worldMat += mul(vin.Weights.z, cFinalTransforms[vin.Indices.z]);
    worldMat += mul(vin.Weights.w, cFinalTransforms[vin.Indices.w]);
    worldMat = mul(worldMat, cWorld);
#elif defined INSTANCING
    worldMat = vin.World;
#else
    worldMat = cWorld;
#endif 
    
    vout.PositionH = mul(float4(vin.Position, 1.f), worldMat);
    vout.PositionH = mul(vout.PositionH, cViewProj);
    
    vout.PositionW = mul(float4(vin.Position, 1.f), worldMat);
    vout.DepthView = mul(float4(vout.PositionW, 1.f), cView).z;
    
    vout.NormalW = normalize(mul(vin.NormalL, (float3x3) worldMat));
    vout.NormalV = normalize(mul(vout.NormalW, (float3x3) cView));
    
    vout.TangentW = normalize(mul(vin.TangentL, (float3x3) worldMat));
    vout.TangentV = normalize(mul(vout.TangentW, (float3x3) cView));
    
    vout.UV = vin.UV;
    
    vout.ClipSpacePosZ = vout.PositionH.z;
    
    return vout;
}