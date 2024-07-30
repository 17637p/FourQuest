#ifndef DECALCOMMON_HLSLI
#define DECALCOMMON_HLSLI

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosClip : POSITION;
    float2 UV : TEXCOORD0;
    float3 Orientation : TEXCOORD1;
};

cbuffer cbDecalObject : register(b0)
{
    matrix gTexMatrix;
    matrix gWorldMatrix;
    matrix gViewMatrix;
    matrix gProjMatrix;
    matrix gInvWVMatrix;
    
    float2 gDeproject;
    float gNormalThresholdInRadian;
}

cbuffer cbDecalMaterial : register(b1)
{
    float4 gBaseColor;
    float4 gEmissiveColor;
    
    bool gUseAlbedoMap;
    bool gUseNormalMap;
    bool gUseEmissiveMap;
    float gNormalBlend;
    
    float gAlphaCutoff;
}

#endif  /* DECALCOMMON_HLSLI */