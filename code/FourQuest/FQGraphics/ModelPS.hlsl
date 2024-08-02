#include "Common.hlsli"

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float ClipSpacePosZ : TEXCOORD2;
};

cbuffer cbMaterial : register(b0)
{
    ModelMaterial gModelMaterial;
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

cbuffer cbShadowTransformCascaseEnd : register(b2)
{
    matrix cLightViewProj[CascadeCount * MaxDirectionalShadowCount];
    float4 cCascadeEnds[CascadeCount];
    int cShadowCount;
}

SamplerState gSamplerAnisotropic : register(s0); //	D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP
SamplerState gLinearClamp : register(s1); // D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP
SamplerComparisonState gShadowSampler : register(s2);

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

float4 main(VertexOut pin) : SV_TARGET
{
    float4 color = gModelMaterial.BaseColor;
    
    if (gModelMaterial.UseAlbedoMap)
    {
        color *= gAlbedoMap.Sample(gSamplerAnisotropic, pin.UV);
    }
    
    clip(color.a - gModelMaterial.AlphaCutoff);
    
    float3 albedo = color.rgb;
    float metalness = gModelMaterial.Metalness;

    if (gModelMaterial.UseMetalnessMap)
    {
        metalness = gMetalnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }

    float roughness = gModelMaterial.Roughness;

    if (gModelMaterial.UseRoughnessMap)
    {
        roughness = gRoughnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }
    
    float3 normal = normalize(pin.NormalW);
    
    if (gModelMaterial.UseNormalMap)
    {
        normal = gNormalMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
        normal = normalize(NormalSampleToWorldSpace(normal, pin.NormalW, pin.TangentW));
    }
    
    float3 emissive = gModelMaterial.EmissiveColor.rgb;
    
    if (gModelMaterial.UseEmissiveMap)
    {
        emissive *= gEmissiveMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    }
    
    if (gModelMaterial.UseMetalnessSmoothness)
    {
        float2 metalnessSmoothness = gMetalnessSmoothness.Sample(gSamplerAnisotropic, pin.UV).xy;
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
            float3 currentDirectLighting = ComputeDirectionLight(directionalLights[i], material, toEye, normal);
            
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
 
                uint shadowIndex = i * CascadeCount + index;
                float4 shadowPos = mul(float4(pin.PositionW, 1.f), cLightViewProj[shadowIndex]);
                shadowPos.x = shadowPos.x * 0.5f + 0.5f;
                shadowPos.y = shadowPos.y * -0.5f + 0.5f;
                shadowRatio = CalculateCascadeShadowRatio(gShadowSampler, gDirectionalShadowMap, shadowPos, shadowIndex, ShadowMapWidth);
                
                currentDirectLighting *= shadowRatio;
            }
            
            directLighting += currentDirectLighting;
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
    
    // // 이게 뒤에 짤린 절두체에는 들어오고 그려질 때 처리하는 건데 이러면 어차피 벗어나는 건 어차피 생기진 않나...?!?!?! ㅇㄹㅁㄴㅇㄹㅁㄴㅇㄹㅁㄴㅇㄹ
    // if (index < 2)
    // {
    //     shadowPos = mul(float4(pin.PositionW, 1.f), cLightViewProj[index + 1]);
    //     float shadowRatio1 = CalculateCascadeShadowRatio(gShadowSampler, gDirectionalShadowMap, shadowPos, index + 1, 2048.f);
    //     shadowRatio = min(shadowRatio, shadowRatio1);
    // }
     //
     //if (index == 0)
     //{
     //    emissive = float3(0.1f, 0.f, 0.f);
     //}
     //else if (index == 1)
     //{
     //    emissive = float3(0, 0.1f, 0);
     //}
     //else if (index == 2)
     //{
     //    emissive = float3(0, 0, 0.1f);
     //}
   
    return float4(directLighting + ambientLighting + emissive, 1.f);
    //return float4(albedo, 1.f);
}