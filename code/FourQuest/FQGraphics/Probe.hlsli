#include "Common.hlsli"

struct LightProbe
{
    float4 cAr;
    float4 cAg;
    float4 cAb;
    float4 cBr;
    float4 cBg;
    float4 cBb;
    float4 cC;
};

float3 ShadeIrad(float4 vNormal, LightProbe probe)
{
    float3 x1, x2, x3;
    
    // Linear + constant polynomial terms
    x1.r = dot(probe.cAr, vNormal);
    x1.g = dot(probe.cAg, vNormal);
    x1.b = dot(probe.cAb, vNormal);
    
    // 4 of the quadratic polynomials
    float4 vB = vNormal.xyzz * vNormal.yzzx;
    x2.r = dot(probe.cBr, vB);
    x2.g = dot(probe.cBg, vB);
    x2.b = dot(probe.cBb, vB);
    
    // Final quadratic polynomial
    float vC = vNormal.x * vNormal.x - vNormal.y * vNormal.y;
    x3 = probe.cC.rgb * vC;
    return x1 + x2 + x3;
}


