
struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

Texture2D gAlbedo : register(t0);
SamplerState gLinearWrap : register(s0);

float4 main(VertexOut pin) : SV_TARGET
{
    float4 albedo = gAlbedo.Sample(gLinearWrap, pin.UV);
    albedo.rgb *= albedo.a;
    
    clip(albedo.a - 0.1f);
    
    return albedo;
}