#ifndef DECALCOMMON_HLSLI
#define DECALCOMMON_HLSLI

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosClip : POSITION;
    float2 UV : TEXCOORD0;
    float3 Orientation : TEXCOORD1;
};

cbuffer perFrame : register(b0)
{
    matrix gTexMatrix;
    matrix gWorldMatrix;
    matrix gViewMatrix;
    matrix gProjMatrix;
    matrix gInvWVMatrix;
    
    float2 gDeproject;
    float gNormalThresholdInRadian;
    float gAlphaClipThreshold;
    
    bool gUseMultiplyAlpha;
    bool gUseAlphaClip;
    bool gUseAlbedoMap;
    bool gUseMetalnessMap;
    
    bool gUseRoughnessMap;
    bool gUseNormalMap;
    bool gUseEmissiveMap;
}

#endif  /* DECALCOMMON_HLSLI */