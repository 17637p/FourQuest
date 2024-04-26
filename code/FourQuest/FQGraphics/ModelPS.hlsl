#include "Common.hlsli"

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float4 PositionShadowTex : TEXCOORD2;
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

static const uint NumLights = 3;

cbuffer cbSceneLight : register(b1)
{
    struct
    {
        float3 Direction;
        float3 Intensity;
    } cLights[NumLights];
    
    float3 cEyePosition;
    bool cUseIBL;
};

//cbuffer cbLight : register(b2)
//{
//    DirectionalLight directionalLights[3];
//    PointLight pointLights[10];
//    SpotLight spotLights[5];
//    
//    unsigned int numOfDirectionalLight;
//	unsigned int numOfPointLight;
//	unsigned int numOfSpotLight;
//	unsigned int isUseIBL;
//
//    float3 eyePosition;
//	float pad2;
//};

SamplerState gSamplerAnisotropic : register(s0); //	D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP
SamplerState gSP_BRDF_LUT_Sampler : register(s1); // D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gNormalMap : register(t3);
Texture2D gEmissiveMap : register(t4);
Texture2D gOpacityMap : register(t5);
TextureCube gDiffuseCubMap : register(t6);
TextureCube gSpecularCubeMap : register(t7);
Texture2D gSpecularBRDF_LUT : register(t8);

static const float3 Fdielectric = 0.04;

float4 main(VertexOut pin) : SV_TARGET
{
    float opacity = 1.f;
    
    if (cUseOpacityMap)
    {
        opacity = gOpacityMap.Sample(gSamplerAnisotropic, pin.UV).r;
        clip(opacity - 0.1f);
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
    
    float3 emissive = 1.f;

    if (cUseEmissiveMap)
    {
        emissive = gEmissiveMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }

    float3 toEye = normalize(cEyePosition - pin.PositionW);
    float ndotv = max(0.f, dot(normal, normalize(toEye)));
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    
    float3 directLighting = 0.0;
    float3 pointLighting = 0.0;
    float3 spotLighting = 0.0;

	[unroll]
    for (uint i = 0; i < NumLights; ++i)
    {
        float3 lightInv = -cLights[i].Direction;
        
        float3 halfVec = normalize(lightInv + toEye);
    
        float ndotl = max(0.0, dot(normal, lightInv));
        float ndoth = max(0.0, dot(normal, halfVec));
    
        float3 F = fresnelSchlick(F0, max(0.0, dot(halfVec, toEye))); // 프레넬
        float D = ndfGGX(ndoth, max(Epsilon, roughness)); // 정반사 분포도
        float G = gaSchlickGGX(ndotl, ndotv, roughness); // 기하 감쇠율
        float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * ndotl * ndotv);
    
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
        float3 diffuseBRDF = kd * albedo / PI;
    
        directLighting += (diffuseBRDF + specularBRDF) * cLights[i].Intensity * ndotl;
    }
    
    //// temp Material
    //Material material;
    //material.albedo = albedo;
    //material.roughness = roughness;
    //material.metallic = metalness;
    //
    //// DirectionalLight 처리
    //for (uint i = 0; i < numOfDirectionalLight; ++i)
    //{
    //    directLighting += ComputeDirectionLight(directionalLights[i], material, toEye, pin.NormalW);
    //}
    //
    //// PointLight 처리
    //for (uint i = 0; i < numOfPointLight; ++i)
    //{
    //    pointLighting += ComputePointLight(pointLights[i], material, toEye, pin.NormalW, pin.PositionW);
    //}
    //
    //// SpotLight 처리
    //for (uint i = 0; i < numOfSpotLight; ++i)
    //{
    //    spotLighting += ComputeSpotLight(spotLights[i], material, toEye, pin.NormalW, pin.PositionW);
    //}
    
    // ibl
    float3 ambientLighting = 0.f;
    
    if (cUseIBL)
    {
        float3 irradiance = gDiffuseCubMap.Sample(gSamplerAnisotropic, normal).rgb;
        float3 F = fresnelSchlick(F0, ndotv);
        float3 kd = lerp(1.0 - F, 0.0, metalness);
        float3 diffuseIBL = kd * albedo * irradiance;
        
        float3 viewReflect = 2.0 * ndotv * normal - toEye;
        uint specularTextureLevels = QueryTextureLevels(gSpecularCubeMap);
        float3 specularIrradiance = gSpecularCubeMap.SampleLevel(gSamplerAnisotropic, viewReflect, roughness * specularTextureLevels).rgb;
        float2 specularBRDF = gSpecularBRDF_LUT.Sample(gSP_BRDF_LUT_Sampler, float2(ndotv, roughness)).rg;
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        ambientLighting = diffuseIBL + specularIBL;
    }
    
    return float4(directLighting + pointLighting + spotLighting + ambientLighting, opacity);
}