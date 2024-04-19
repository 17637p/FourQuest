struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float4 PositionShadowTex : TEXCOORD2;
};

SamplerState gSamLinear;

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gNormalMap : register(t3);
Texture2D gEmissiveMap : register(t4);
Texture2D gOpacityMap : register(t5);

float4 main(VertexOut pin) : SV_TARGET
{
    float4 albedo = gAlbedoMap.Sample(gSamLinear, pin.UV);
    
    return albedo;
}