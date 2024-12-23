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
    float DepthView : TEXCOORD3;
    float3 NormalV : TEXCOORD4;
    float3 TangentV : TEXCOORD5;
    float2 BlendUV : TEXCOORD6;
#ifdef STATIC
    float2 UV1 : TEXCOORD7;    
    uint LightmapIndex : TEXCOORD8;
#endif
#ifdef VERTEX_COLOR
    float4 Color : COLOR0;
#endif
};

struct PixelOut
{
    float4 Accum : SV_Target0;
    float Reveal : SV_Target1;
};

cbuffer cbMaterial : register(b0)
{
    ModelMaterial gModelMaterial;
};

cbuffer cbLight : register(b1)
{
    DirectionalLight directionalLights[3];
    PointLight pointLights[30];
    SpotLight spotLights[5];
    
    unsigned int numOfDirectionalLight;
    unsigned int numOfPointLight;
    unsigned int numOfSpotLight;
    unsigned int isUseIBL;

    float3 eyePosition;
    float pad2;
};

cbuffer cbMaterialInstance : register(b2)
{
    bool cUseAlpha;
    float cAlpha;
    bool cUseDissolveCutoff;
    float cDissolveCutoff;

    int cUseRimLight;
    float cRimPow;
    int cUseInvRimLight;
    float cInvRimPow;

    float4 cRimColor;
    float4 cInvRimColor;

    float2 cUVScale;
    float2 cUVOffset;

    int cUseScaleOffset;
    float cRimIntensity;
    float cInvRimIntensity;
};

cbuffer cbDirectionalShadow : register(b3)
{
    matrix cLightViewProj[CascadeCount * MaxDirectionalShadowCount];
    float4 cCascadeEnds[CascadeCount];
    int cShadowCount;
}

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gNormalMap : register(t3);
Texture2D gEmissiveMap : register(t4);
Texture2D gMetalnessSmoothness : register(t5);

TextureCube gDiffuseCubMap : register(t6);
TextureCube gSpecularCubeMap : register(t7);
Texture2D gSpecularBRDF_LUT : register(t8);
Texture2DArray gDirectionalShadowMap : register(t9);

Texture2D gNoiseMap : register(t10);
Texture2D gBlendMap : register(t11);

SamplerState gSamplerAnisotropic : register(s0); 
SamplerComparisonState gShadowSampler : register(s1);
SamplerState gLinearClamp : register(s2); 
SamplerState gLinearWrap : register(s3); 

PixelOut main(VertexOut pin) : SV_TARGET
{
    PixelOut pout;

#ifdef VERTEX_COLOR
    float4 baseColor = gModelMaterial.BaseColor;
    baseColor.a *= pin.Color.x; // 알파값으로만 사용할 것이라 임의로 x로 적용
#else
    float4 baseColor = gModelMaterial.BaseColor;
#endif

    if (gModelMaterial.UseAlbedoMap)
    {
        baseColor *= gAlbedoMap.Sample(gSamplerAnisotropic, pin.UV);
    }

    float3 albedo = baseColor.rgb;
    float opacity = baseColor.a;
    clip(opacity - gModelMaterial.AlphaCutoff);
    
    float metalness = 0.f;

    if (gModelMaterial.UseMetalnessMap)
    {
        metalness = gMetalnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }
    else
    {
        metalness = gModelMaterial.Metalness;
    }

    float roughness = 0.f;

    if (gModelMaterial.UseRoughnessMap)
    {
        roughness = gRoughnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }
    else
    {
        roughness = gModelMaterial.Roughness;
    }

    float3 normal = normalize(pin.NormalW);
    
    if (gModelMaterial.UseNormalMap)
    {
        normal = gNormalMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
        normal = normalize(NormalSampleToWorldSpace(normal, pin.NormalW, pin.TangentW));
    }
    
    float3 emissive = gModelMaterial.EmissiveColor.rgb * gModelMaterial.EmissiveIntensity;
    
    if (gModelMaterial.UseEmissiveMap)
    {
        emissive *= gEmissiveMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    }
    
    if (gModelMaterial.UseMulEmissiveAlpha)
    {    
        emissive *= opacity;
    }

    if (gModelMaterial.UseDissolve)
    {
        float4 noise = gNoiseMap.Sample(gLinearWrap, pin.UV);
        float dissolveCutoff = gModelMaterial.DissolveCutoff;

        clip(noise.x - dissolveCutoff);

        float outlineWeight = saturate(dissolveCutoff * gModelMaterial.OutlineThickness - noise.r);
        float3 outlineColor = lerp(gModelMaterial.DissolveOutlineStartColor.rgb, gModelMaterial.DissolveOutlineEndColor.rgb ,outlineWeight);
        float4 outlineEmissive = lerp(gModelMaterial.DissolveOutlineStartEmissive, gModelMaterial.DissolveOutlineEndEmissive ,outlineWeight);        

        if(noise.r > dissolveCutoff * gModelMaterial.OutlineThickness)
        {
            outlineColor *= 0;
            outlineEmissive *= 0;
        }        
        else
        {
            outlineColor *= 1;
            outlineEmissive *= 1;
        }

        albedo += outlineColor;
        emissive += outlineEmissive.rgb;
    }

    if (gModelMaterial.UseMetalnessSmoothness)
    {
        float2 metalnessSmoothness = gMetalnessSmoothness.Sample(gSamplerAnisotropic, pin.UV).xw;
        metalness = metalnessSmoothness.x;
        roughness = 1 - metalnessSmoothness.y;
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
 
                float4 shadowPos = mul(float4(pin.PositionW, 1.f), cLightViewProj[index]);
                shadowPos.x = shadowPos.x * 0.5f + 0.5f;
                shadowPos.y = shadowPos.y * -0.5f + 0.5f;
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

    if (gModelMaterial.bUseRimLight)
    {
        float3 toEye = normalize(eyePosition - pin.PositionW.xyz);
        float rim = saturate(dot(normal, toEye));
        rim = pow(1 - rim, gModelMaterial.RimPow);
        emissive.rgb += rim * gModelMaterial.RimColor.rgb * gModelMaterial.RimIntensity;
    }
    if (gModelMaterial.bUseInvRimLight)
    {
        float3 toEye = normalize(eyePosition - pin.PositionW.xyz);
        float rim = saturate(dot(normal, toEye));
        rim = pow(rim, gModelMaterial.InvRimPow);
        emissive.rgb += rim * gModelMaterial.InvRimColor.rgb * gModelMaterial.InvRimIntensity;
    }

    if (gModelMaterial.bUseBlendTexture)
    {
        float4 blendColor = gBlendMap.Sample(gSamplerAnisotropic, pin.BlendUV);

        if (gModelMaterial.bIsBlendBaseColor)    
        {
            directLighting *= blendColor.rgb;
            ambientLighting *= blendColor.rgb   ;
            opacity *= blendColor.a;
        }

        if (gModelMaterial.bIsBlendEmissive)    
        {
            emissive *= blendColor;
        }

        clip(opacity - gModelMaterial.AlphaCutoff);
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
