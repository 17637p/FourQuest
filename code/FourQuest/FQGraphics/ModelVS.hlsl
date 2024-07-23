
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
#ifdef STATIC
    float2 UV1 : UV1;
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
#ifdef STATIC
    float2 UV1 : TEXCOORD6;
#endif
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

cbuffer cbMaterial : register(b3)
{
    float4 cBaseColor;
    float4 cEmissiveColor;
    float4x4 gTexTransform;
    
    float cMetalness;
    float cRoughness;
    bool cUseAlbedoMap;
    bool cUseMetalnessMap;
  
    bool cUseRoughnessMap;
    bool cUseNormalMap;
    bool cUseEmissiveMap;
};

#ifdef STATIC
cbuffer cbLightmapInformation : register(b4)
{
    float4 cUVOffsetScale;
    uint cUVIndex;
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
    
    vout.UV = mul(float4(vin.UV, 0, 1), gTexTransform);
    
    vout.ClipSpacePosZ = vout.PositionH.z;
    
#ifdef STATIC
    vin.UV1.y = 1 - vin.UV1.y; 
    vout.UV1 = vin.UV1 * cUVOffsetScale.xy + cUVOffsetScale.zw;
    vout.UV1.y = 1 - vout.UV1.y; 
#endif
    
    return vout;
}