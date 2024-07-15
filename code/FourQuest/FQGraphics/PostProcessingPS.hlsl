#include "common.hlsli"

float3 ReinhardToneMapping(float3 color)
{
    return color / (color + 1.0);
}

float3 ACESFilm(float3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// https://catlikecoding.com/unity/tutorials/custom-srp/color-grading/ 참고
float3 AdjustExposure(float3 color, float exposure)
{
    exposure = pow(exposure, 2.f);
    color.rgb *= exposure;
    return color;
}

float3 AdjustContrast(float3 color, float contrast)
{
    contrast = contrast * 0.01f + 1.f;
    float3 mean = float3(0.5, 0.5, 0.5); // 평균 색상 값 (중간 값)
    color.rgb = (color.rgb - mean) * contrast + mean;
    return color;
}

float3 AdjustSaturation(float3 color, float saturation)
{
    saturation = saturation * 0.01f + 1.f;
    float3 gray = GetBrightness(color); // 회색조 값
    color.rgb = lerp(gray, color.rgb, saturation);
    return color;
}

float SoftLight(float base, float blend)
{
    return (blend < 0.5) ? (2 * base * blend + base * base * (1 - 2 * blend)) :
                           (sqrt(base) * (2 * blend - 1) + 2 * base * (1 - blend));
}
float3 SoftLight(float3 base, float3 blend)
{
    return float3(SoftLight(base.r, blend.r), SoftLight(base.g, blend.g), SoftLight(base.b, blend.b));
}

float3 AdjustSplitToning(float3 color, float3 shadowColor, float3 highlightColor, float balance)
{
    float t = saturate(GetBrightness(saturate(color)) + balance);
    float3 shadows = lerp(0.5, shadowColor.rgb, 1.0 - t);
    float3 highlights = lerp(0.5, highlightColor.rgb, t);
    
    color = SoftLight(color, shadows);
    color = SoftLight(color, highlights);
    return color;
}

float3 AdjustVignetting(float3 color, float radius, float smoothness, float2 uv, float4 vigentteColor)
{
    uv -= 0.5;
    float dist = length(uv);
    float3 vignette = smoothstep(radius, radius - smoothness, dist) + vigentteColor.rgb;
    return lerp(color, color * vignette, vigentteColor.a);
}

cbuffer cColorAdjustment : register(b0)
{
    float4 gBloomColorTint;
    float4 gShadowColor;
    float4 gHighlightColor;
    float4 gVignettColor;
    
    float gExposure;
    float gContrast;
    float gSaturation;
    float gGamma;
    
    float gBloomIntensity;
    float gBloomThreshold;
    float gBalance;
    float gVignettRadius;
    
    float gVignettSmoothness;
    int bUseColorAdjustment;
    int bUseBloom;
    int bUseSplitToning;

    int bUseVignett;
    int bUseToneMapping;
    int bUseHueVsSatCurve;
};

Texture2D gSrcTexture : register(t0);
Texture2D gBloomTexture : register(t1);
Texture1D gHutVsSatCurveTexture : register(t2);
SamplerState gSamplerPoint : register(s0);
SamplerState gSamplerLinear : register(s1);

#define BLOOMCURVE_METHOD_1
//#define BLOOMCURVE_METHOD_2
// #define BLOOMCURVE_METHOD_3

float GetBloomCurve(float x, float threshold)
{
    float result = x;
    x *= 2.0f;
    
#ifdef BLOOMCURVE_METHOD_1
    result = x * 0.05 + max(0, x - threshold) * 0.5; // default threshold = 1.26
#endif
    
#ifdef BLOOMCURVE_METHOD_2
    result = x*x/3.2;
#endif
    
#ifdef BLOOMCURVE_METHOD_3
    result = max(0, x - threshold); // default threshold = 1.0
    result *= result;
#endif 
    
    return result * 0.5f;
}

float4 main(float2 uv : Texcoord) : SV_TARGET
{
    float3 color = gSrcTexture.Sample(gSamplerPoint, uv).rgb;
    
    if (bUseBloom)
    {
        float3 bloomColor = gBloomTexture.Sample(gSamplerLinear, uv).rgb;
        color += (bloomColor * GetBloomCurve(gBloomIntensity, gBloomThreshold) * gBloomColorTint.rgb);
    }
    
    if (bUseColorAdjustment)
    {
        color = min(color, 60.f);
        color = AdjustExposure(color, gExposure);
        color = AdjustContrast(color, gContrast);
        color = max(color, 0);
        color = AdjustSaturation(color, gSaturation);
    }
    
    if (bUseToneMapping)
    {
        color = ReinhardToneMapping(color);
    }
    
    if (bUseSplitToning)
    {
        color = AdjustSplitToning(color, gShadowColor.rgb, gHighlightColor.rgb, gBalance);
    }

    if (bUseVignett)
    {
        color = AdjustVignetting(color, gVignettRadius, gVignettSmoothness, uv, gVignettColor);
    }
    
    float3 retColor = pow(color, 1 / gGamma);
    
    return float4(retColor, 1.0);
}