#include "ParticleCommon.hlsli"

Texture2D gAlbedoTexture : register(t0);
Texture2D gEmissiveTexture : register(t1);
Texture2D gDepthTexture : register(t2); // ±íÀÌ ÅØ½ºÃ³

SamplerState gSamWrapLinear : register(s0);
SamplerState gSamClampLinear : register(s1);

struct PS_INPUT
{
    float4 PosH : SV_Position;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
    float4 PosTex : TEXCOORD1;
};

float4 main(PS_INPUT pin) : SV_TARGET
{
    pin.PosTex /= pin.PosTex.w;
    float depth = gDepthTexture.Sample(gSamClampLinear, pin.PosTex.xy).x;

    if (pin.PosH.z > depth)
    {
        clip(-1);
    }
	
    float4 albedo = gParticleRender.BaseColor;
    
    if (gParticleRender.bUseAlbedo)
    {
        albedo *= gAlbedoTexture.SampleLevel(gSamClampLinear, pin.Tex, 0);
    }
    
    float3 emissive = gParticleRender.EmssiveColor.rgb;
    
    if (gParticleRender.bUseEmissive)
    {
        emissive *= gAlbedoTexture.SampleLevel(gSamClampLinear, pin.Tex, 0).rgb;
    }
    
    switch (gParticleRender.ColorMode)
    {
        case COLOR_MODE_MULTIPLY:
            albedo *= pin.Color;
            break;
        case COLOR_MODE_ADDITIVE:
            albedo += pin.Color;
            break;
        case COLOR_MODE_SUBTRACT:
            albedo -= pin.Color;
            break;
        case COLOR_MODE_OVERLAY:
            albedo = OverlayMode(albedo, pin.Color);
            break;
        case COLOR_MODE_COLOR:
            albedo = ColorMode(albedo, pin.Color);
            break;
        case COLOR_MODE_DIFFERENCE:
            albedo = abs(albedo - pin.Color);
            break;
    }
    
    float4 color = float4(albedo.rgb + emissive, albedo.a);
    
    switch (gParticleRender.RenderMode)
    {
        case RENDER_MODE_ADDITIVE:
            color.rgb *= color.a;
            break;
        case RENDER_MODE_SUBTRACTIVE:
            color.rgb *= color.a;
            break;
        case RENDER_MODE_MODULATE:
            color.rgb *= color.a;
            break;
        case RENDER_MODE_OPQUE:
            break;
    }
    
    clip(color.a - gParticleRender.AlphaCutoff);
    
    return color;
}