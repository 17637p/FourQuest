#include "Common.hlsli"
#ifdef RENDER

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float ClipSpacePosZ : TEXCOORD2;
};

struct PixelOut
{
    float4 Accum : SV_Target0;
    float Reveal : SV_Target1;
};

cbuffer cbModelTexture : register(b0)
{
    bool cUseAlbedoMap;
    bool cUseMetalnessMap;
    bool cUseRoughnessMap;
    bool cUseNormalMap;
    bool cUseEmissiveMap;
    bool cUseOpacityMap;
};

cbuffer cbLight : register(b1)
{
    DirectionalLight directionalLights[3];
    PointLight pointLights[10];
    SpotLight spotLights[5];
    
    unsigned int numOfDirectionalLight;
    unsigned int numOfPointLight;
    unsigned int numOfSpotLight;
    unsigned int isUseIBL;

    float3 eyePosition;
    float pad2;
};

cbuffer cbAlpha : register(b2)
{
    bool cUseAlpha;
    float cAlpha;
};

cbuffer cbDirectionalShadow : register(b3)
{
    matrix cLightViewProjTex[CascadeCount * MaxDirectionalShadowCount];
    float4 cCascadeEnds[CascadeCount];
    int cShadowCount;
}

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gNormalMap : register(t3);
Texture2D gEmissiveMap : register(t4);
Texture2D gOpacityMap : register(t5);
TextureCube gDiffuseCubMap : register(t6);
TextureCube gSpecularCubeMap : register(t7);
Texture2D gSpecularBRDF_LUT : register(t8);
Texture2DArray gDirectionalShadowMap : register(t9);

SamplerState gSamplerAnisotropic : register(s0); 
SamplerComparisonState gShadowSampler : register(s1);
SamplerState gLinearClamp : register(s2); 

PixelOut main(VertexOut pin) : SV_TARGET
{
    PixelOut pout;
    float opacity = 1.f;
    
    if (cUseOpacityMap)
    {
        opacity = gOpacityMap.Sample(gSamplerAnisotropic, pin.UV).r;
        clip(opacity - 0.1f);
    }
    
    if (cUseAlpha)
    {
        opacity = cAlpha;
    }

    float3 albedo = float3(1.f, 1.f, 1.f);
    
    if (cUseAlbedoMap)
    {
        albedo = gAlbedoMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    }
    
    float metalness = 0.f;

    if (cUseMetalnessMap)
    {
        metalness = gMetalnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }

    float roughness = 0.f;

    if (cUseRoughnessMap)
    {
        roughness = gRoughnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }
    
    float3 normal = normalize(pin.NormalW);
    
    if (cUseNormalMap)
    {
        normal = gNormalMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
        normal = normalize(NormalSampleToWorldSpace(normal, pin.NormalW, pin.TangentW));
    }
    
    float3 emissive = float3(0.f, 0.f, 0.f);
    
    if (cUseEmissiveMap)
    {
        emissive = gEmissiveMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    }

    float3 directLighting = 0.0;

    Material material;
    material.albedo = albedo;
    material.roughness = roughness;
    material.metallic = metalness;
    float3 toEye = normalize(eyePosition - pin.PositionW);
    
    // DirectionalLight 처리
    {
        for (uint i = 0; i < numOfDirectionalLight; ++i)
        {
            directLighting += ComputeDirectionLight(directionalLights[i], material, toEye, normal);
            
            if (i < cShadowCount)
            {
                float shadowRatio = 1.f;
    
                uint index = 2;
    
                for (uint j = 0; j < 3; ++j)
                {
                    if (pin.ClipSpacePosZ <= cCascadeEnds[i][j])
                    {
                        index = j;
                        break;
                    }
                }
 
                float4 shadowPos = mul(float4(pin.PositionW, 1.f), cLightViewProjTex[index]);
                shadowRatio = CalculateCascadeShadowRatio(gShadowSampler, gDirectionalShadowMap, shadowPos, i * CascadeCount + index, ShadowMapWidth);
                
                directLighting *= shadowRatio;
            }
        }
    }
    
    // PointLight 처리
    {
        for (uint i = 0; i < numOfPointLight; ++i)
        {
            directLighting += ComputePointLight(pointLights[i], material, toEye, normal, pin.PositionW);
        }
    }
    
    // SpotLight 처리
    {
        for (uint i = 0; i < numOfSpotLight; ++i)
        {
            directLighting += ComputeSpotLight(spotLights[i], material, toEye, normal, pin.PositionW);
        }
    }
 
    float ndotv = max(0.f, dot(normal, normalize(toEye)));
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    
    // ibl
    float3 ambientLighting = 0.f;
    
    if (isUseIBL)
    {
        float3 irradiance = gDiffuseCubMap.Sample(gSamplerAnisotropic, normal).rgb;
        float3 F = fresnelSchlick(F0, ndotv);
        float3 kd = lerp(1.0 - F, 0.0, metalness);
        float3 diffuseIBL = kd * albedo * irradiance;
        
        float3 viewReflect = 2.0 * ndotv * normal - toEye;
        uint specularTextureLevels = QueryTextureLevels(gSpecularCubeMap);
        float3 specularIrradiance = gSpecularCubeMap.SampleLevel(gSamplerAnisotropic, viewReflect, roughness * specularTextureLevels).rgb;
        float2 specularBRDF = gSpecularBRDF_LUT.Sample(gLinearClamp, float2(ndotv, roughness)).rg;
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        ambientLighting = diffuseIBL + specularIBL;
    }

    float4 color = float4(directLighting + ambientLighting + emissive, opacity);

    float z = pin.PositionH.z;
    //float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) * clamp(0.03 / (1e-5 + pow(z / 200, 4.0)), 1e-2, 3e3);
    float weight = clamp(pow(min(1.0, color.a * 10.0f) + 0.01, 3.0f) * 1e8 * pow(1.0 - z * 0.9f, 3.0f), 1e-2, 3e3);
    
    pout.Accum = float4(color.rgb * color.a, color.a) * weight;
    pout.Reveal = color.a;
    
    return pout;
}

#elif defined COMPOSITE

Texture2D gAccumMap : register(t0);
Texture2D gRevealMap : register(t1);

SamplerState gSamplerPoint : register(s0);

float4 main(float2 uv : Texcoord) : SV_TARGET
{
    float reveal = gRevealMap.Sample(gSamplerPoint, uv).r;
    
    // 불투명할 경우 랜더링 하지 않음
    if (IsApproximatelyEqual(reveal, 1.0f))
    {
        discard;
    }
    
    float4 accum = gAccumMap.Sample(gSamplerPoint, uv);
    
    // 오버플로우 발생 시 알파값으로 초기화
    if (isinf(max(max(accum.r, accum.g), accum.b)))
    {
        accum.rgb = accum.a;
    }
    
    float3 color = accum.rgb / max(accum.a, Epsilon);
    
    return float4(color, 1.f - reveal);
}
#endif
