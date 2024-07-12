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

float GetBrightness(float3 color)
{
    return dot(color.rgb, float3(0.299, 0.587, 0.114));
}

float3 AdjustExposure(float3 color, float exposure)
{
    color.rgb *= exposure;
    return color;
}

float3 AdjustContrast(float3 color, float contrast)
{
    float3 mean = float3(0.5, 0.5, 0.5); // 평균 색상 값 (중간 값)
    color.rgb = (color.rgb - mean) * contrast + mean;
    return color;
}

float3 AdjustSaturation(float3 color, float saturation)
{
    float3 gray = GetBrightness(color); // 회색조 값
    color.rgb = lerp(gray, color.rgb, saturation);
    return color;
}

float3 AdjustSplitToning(float3 color, float3 shadowColor, float3 highlightColor, float balance)
{
    float brightness = GetBrightness(color);
    float3 result = lerp(shadowColor, highlightColor, smoothstep(balance - 0.1, balance + 0.1, brightness));
    result = lerp(color, result, 0.5);
    return result;
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
};

Texture2D gSrcTexture : register(t0);
Texture2D gBloomTexture : register(t1);
SamplerState gSamplerPoint : register(s1);

#define BLOOMCURVE_METHOD_1
//#define BLOOMCURVE_METHOD_2
// #define BLOOMCURVE_METHOD_3

float GetBloomCurve(float x, float threshold)
{
    float result = x;
    x *= 2.0f;
    
#ifdef BLOOMCURVE_METHOD_1
    result = x*0.05+max(0,x-threshold)*0.5; // default threshold = 1.26
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
        float3 bloomColor = gBloomTexture.Sample(gSamplerPoint, uv).rgb;
        color += bloomColor * GetBloomCurve(gBloomIntensity, gBloomThreshold) * gBloomColorTint.rgb;
    }
    
    if (bUseColorAdjustment)
    {
        color = AdjustExposure(color, gExposure);
        color = AdjustContrast(color, gContrast);
        color = AdjustSaturation(color, gSaturation);
    }
    
    if (bUseSplitToning)
    {
        color = AdjustSplitToning(color, gShadowColor.rgb, gHighlightColor.rgb, saturate(gBalance));
    }

    if (bUseToneMapping)
    {
        color = ReinhardToneMapping(color);
    }
    
    if (bUseVignett)
    {
        color = AdjustVignetting(color, gVignettRadius, gVignettSmoothness, uv, gVignettColor);
    }

    float3 retColor = pow(color, 1 / gGamma);
    
    return float4(retColor, 1.0);
}