#include "Common.hlsli"

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
};

#define RENDER_MODE_OPQUE 0
#define RENDER_MODE_ADDITIVE 1
#define RENDER_MODE_SUBTRACTIVE 2
#define RENDER_MODE_MODULATE 3

#define COLOR_MODE_MULTIPLY 0
#define COLOR_MODE_ADDITIVE 1
#define COLOR_MODE_SUBTRACT 2
#define COLOR_MODE_OVERLAY 3
#define COLOR_MODE_COLOR 4
#define COLOR_MODE_DIFFERENCE 5

cbuffer cbMaterial : register(b0)
{
    ParticleMaterial gParticleMaterial;
};

Texture2D gAlbedoTexture : register(t0);
Texture2D gEmissiveTexture : register(t1);

SamplerState gLinearWrap : register(s0);

float4 main(VertexOut pin) : SV_TARGET
{
    float4 albedo = float4(1, 1, 1, 1);
    
    if (gParticleMaterial.bUseAlbedo)
    {
        float gamma = 2.2f;
        albedo = gAlbedoTexture.SampleLevel(gLinearWrap, pin.Tex, 0);
        albedo = pow(albedo, gamma);
    }
    
    albedo *= gParticleMaterial.BaseColor;
    
    float3 emissive = float3(1, 1, 1);
    
    if (gParticleMaterial.bUseEmissive)
    {
        emissive = gAlbedoTexture.SampleLevel(gLinearWrap, pin.Tex, 0).rgb;
    }
    
    emissive *= gParticleMaterial.EmssiveColor.rgb;
    
    switch (gParticleMaterial.ColorMode)
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
    
    switch (gParticleMaterial.RenderMode)
    {
        case RENDER_MODE_OPQUE:
            break;
        case RENDER_MODE_ADDITIVE:
            color.rgb *= color.a;
            break;
        case RENDER_MODE_SUBTRACTIVE:
            color.rgb *= color.a;
            break;
        case RENDER_MODE_MODULATE:
            color.rgb *= color.a;
            break;
    }
    
    clip(color.a - gParticleMaterial.AlphaCutoff);
    
    return albedo;
}