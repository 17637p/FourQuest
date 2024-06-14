
struct VertexIn
{
    float3 Position : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 UV : UV;
#ifdef SKINNING
    int4 Indices : INDICES;
    float4 Weights : WEIGHTS;
#endif
};

struct VertexOut
{
    float4 PositionW : SV_POSITION;
};

cbuffer cbModelTransform : register(b0)
{
    float4x4 cWorld;
    float4x4 cWorldInvTranspose;
};

#ifdef SKINNING
cbuffer cbBoneTransform : register(b1)
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
#else
    worldMat = cWorld;
#endif 
    
    vout.PositionW = mul(float4(vin.Position, 1.f), worldMat);
    
    return vout;
}
