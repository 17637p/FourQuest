#include "ParticleCommon.hlsli"

Texture2D gParticleTexture : register(t0);
Texture2D gDepthTexture : register(t1); // ±íÀÌ ÅØ½ºÃ³

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
	
    float4 albedo = float4(1, 1, 1, 1);
    
    if (gParticleRender.bHasTexture)
    {
        float gamma = 2.2f;
        albedo = gParticleTexture.SampleLevel(gSamClampLinear, pin.Tex, 0);
        albedo = pow(albedo, gamma);
    }
    
    float4 color = albedo * pin.Color;
    
    if (gParticleRender.bUseAlphaClip)
    {
        clip(color.a - gParticleRender.AlphaClipThreshold);
    }
    
    if (gParticleRender.bUseMultiplyAlpha)
    {
        color.rgb *= color.a;

    }
    
    return color;
    
    // float z = pin.PosH.z;
    // float weight = clamp(pow(min(1.0, color.a * 10.0f) + 0.01, 3.0f) * 1e8 * pow(1.0 - z * 0.9f, 3.0f), 1e-2, 3e3);
    // PixelOut pout;
    // 
    // // over
    // pout.Accum = float4(color.rgb * color.a, color.a) * weight;
    // pout.Reveal = color.a;
    // 
    // return pout;
}