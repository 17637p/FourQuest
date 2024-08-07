#include "Common.hlsli"

struct DomainOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
    float2 TiledTex : TEXCOORD1;
    float ClipSpacePosZ : TEXCOORD2;
};

struct Layer
{
    float TileSizeX;
    float TileSizeY;
    float TileOffsetX;
    float TileOffsetY;

    float Metalic;
    float Roughness;
    float a;
    float b;
};

cbuffer cbTerrainTexture : register(b0)
{
    int NumOfTexture;
    float3 padTerrainTexture;

    Layer layers[4];
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

cbuffer cbDirectionalShadow : register(b2)
{
    matrix cLightViewProj[CascadeCount * MaxDirectionalShadowCount];
    float4 cCascadeEnds[CascadeCount];
    int cShadowCount;
}

cbuffer cbLightmapInformation : register(b3)
{
    float4 cUVOffsetScale;
    uint cUVIndex;
    bool bUseLightmap;
    bool bUseDirectMap;
};

float gTexelCellSpaceU;
float gTexelCellSpaceV;

float gWorldCellSpace;

SamplerState gSamplerAnisotropic : register(s0);
SamplerComparisonState gShadowSampler : register(s1);
SamplerState samHeightmap : register(s2);
SamplerState gPointWrap : register(s3); //	D3D11_FILTER_POINT, D3D11_TEXTURE_ADDRESS_WRAP
SamplerState gLinearWrap : register(s4); //	D3D11_FILTER_Linear, D3D11_TEXTURE_ADDRESS_WRAP

Texture2D gAlbedoMap[4] : register(t0);
//Texture2D gAlbedoMap2 : register(t1);
//Texture2D gAlbedoMap3 : register(t2);
//Texture2D gAlbedoMap4 : register(t3);

Texture2D gMetalnessMap1 : register(t4);
Texture2D gMetalnessMap2 : register(t5);
Texture2D gMetalnessMap3 : register(t6);
Texture2D gMetalnessMap4 : register(t7);

Texture2D gRoughnessMap1 : register(t8);
Texture2D gRoughnessMap2 : register(t9);
Texture2D gRoughnessMap3 : register(t10);
Texture2D gRoughnessMap4 : register(t11);

Texture2D gNormalMap[4] : register(t12);
//Texture2D gNormalMap2 : register(t13);
//Texture2D gNormalMap3 : register(t14);
//Texture2D gNormalMap4 : register(t15);

Texture2D gAlphaMap : register(t16);
Texture2D gHeightMap : register(t17);
Texture2DArray gDirectionalShadowMap : register(t18);

Texture2DArray gLightMapArray : register(t19);
Texture2DArray gDirectionArray : register(t20);

#ifdef DEFERRED

struct PixelOut
{
    float4 Albedo : SV_Target0;
    float2 MetalnessRoughness : SV_Target1;
    float4 Normal : SV_Target2;
    float4 Emissive : SV_Target3;
    float4 PositionW : SV_Target4;
    float4 SourceNormal : SV_Target5;
    float4 SourceTangent : SV_Target6;
    float4 Light : SV_Target7;
};

#else

struct PixelOut
{
    float4 Color : SV_TARGET;
};
    
#endif

PixelOut main(DomainOut pin)
{
    PixelOut pout = (PixelOut) 0;
    
    float2 leftTex = pin.UV + float2(-gTexelCellSpaceU, 0.0f);
    float2 rightTex = pin.UV + float2(gTexelCellSpaceU, 0.0f);
    float2 bottomTex = pin.UV + float2(0.0f, gTexelCellSpaceV);
    float2 topTex = pin.UV + float2(0.0f, -gTexelCellSpaceV);

    float leftY = gHeightMap.SampleLevel(samHeightmap, leftTex, 0).r;
    float rightY = gHeightMap.SampleLevel(samHeightmap, rightTex, 0).r;
    float bottomY = gHeightMap.SampleLevel(samHeightmap, bottomTex, 0).r;
    float topY = gHeightMap.SampleLevel(samHeightmap, topTex, 0).r;

    float3 tangent = normalize(float3(2.0f * gWorldCellSpace, rightY - leftY, 0.0f));
    float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f * gWorldCellSpace));
    float3 normalW = cross(tangent, bitan);
    
    
    //float3 albedo = float3(1.f, 1.f, 1.f);
    //
    //if (cUseAlbedoMap)
    //{
    //    albedo = gAlbedoMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    //}
    //
    //float metalness = 0.f;
    //
    //if (cUseMetalnessMap)
    //{
    //    metalness = gMetalnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    //}
    //
    //float roughness = 0.f;
    //
    //if (cUseRoughnessMap)
    //{
    //    roughness = gRoughnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    //}
    //
    //float3 normal = normalize(pin.NormalW);
    //
    //if (cUseNormalMap)
    //{
    //    normal = gNormalMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    //    normal = normalize(NormalSampleToWorldSpace(normal, pin.NormalW, pin.TangentW));
    //}   
    // 텍스처 블렌딩
    float3 resultAlbedo = { 0, 0, 0 };
    float3 resultNormal = { 0, 0, 0 };
    float3 resultMetalic = 0;
    float3 resultRoughness = 0;
    float4 alpha = pow(gAlphaMap.Sample(gSamplerAnisotropic, pin.UV), 1);

    float2 tileUV[4];
    
    for (unsigned int i = 0; i < NumOfTexture; i++)
    {
        tileUV[i].x = (layers[i].TileOffsetX + pin.UV.x) * layers[i].TileSizeX;
        tileUV[i].y = (layers[i].TileOffsetY + pin.UV.y) * layers[i].TileSizeY;
    }

    float3 albedo4 = pow(gAlbedoMap[3].Sample(gSamplerAnisotropic, tileUV[3]), 2.2);
    float3 albedo3 = pow(gAlbedoMap[2].Sample(gSamplerAnisotropic, tileUV[2]), 2.2);
    float3 albedo2 = pow(gAlbedoMap[1].Sample(gSamplerAnisotropic, tileUV[1]), 2.2);
    float3 albedo1 = pow(gAlbedoMap[0].Sample(gSamplerAnisotropic, tileUV[0]), 2.2);

    float3 normal4 = gNormalMap[3].Sample(gSamplerAnisotropic, tileUV[3]);
    normal4 = normalize(NormalSampleToWorldSpace(normal4, pin.NormalW, pin.TangentW));
    float3 normal3 = gNormalMap[2].Sample(gSamplerAnisotropic, tileUV[2]);
    normal3 = normalize(NormalSampleToWorldSpace(normal3, pin.NormalW, pin.TangentW));
    float3 normal2 = gNormalMap[1].Sample(gSamplerAnisotropic, tileUV[1]);
    normal2 = normalize(NormalSampleToWorldSpace(normal2, pin.NormalW, pin.TangentW));
    float3 normal1 = gNormalMap[0].Sample(gSamplerAnisotropic, tileUV[0]);
    normal1 = normalize(NormalSampleToWorldSpace(normal1, pin.NormalW, pin.TangentW));

    if (NumOfTexture > 3)
    {
        resultAlbedo += albedo4 * alpha.a;
        resultNormal += normal4 * alpha.a;
        resultMetalic += layers[3].Metalic * alpha.a;
        resultRoughness += layers[3].Roughness * alpha.a;
    }
    if (NumOfTexture > 2)
    {
        resultAlbedo += albedo3 * alpha.b;
        resultNormal += normal3 * alpha.b;
        resultMetalic += layers[2].Metalic * alpha.b;
        resultRoughness += layers[2].Roughness * alpha.b;
    }
    if (NumOfTexture > 1)
    {
        resultAlbedo += albedo2 * alpha.g;
        resultNormal += normal2 * alpha.g;
        resultMetalic += layers[1].Metalic * alpha.g;
        resultRoughness += layers[1].Roughness * alpha.g;
    }
    if (NumOfTexture > 0)
    {
        resultAlbedo += albedo1 * alpha.r;
        resultNormal += normal1 * alpha.r;
        resultMetalic += layers[0].Metalic * alpha.r;
        resultRoughness += layers[0].Roughness * alpha.r;
    }

    resultAlbedo = pow(resultAlbedo, 1 / 2.2);
    resultNormal = normalize(resultNormal);
     
    
#ifdef DEFERRED
    pout.Normal.xyz = resultNormal;
    pout.Albedo.xyz = resultAlbedo;
    pout.Albedo.w = 1.f;
    pout.MetalnessRoughness.x = resultMetalic;
    pout.MetalnessRoughness.y = resultRoughness;
    pout.Emissive = float4(0.f, 0.f, 0.f, 0.f);
    pout.SourceNormal.xyz = pin.NormalW;
    pout.SourceTangent.xyz = pin.TangentW;
    pout.PositionW.xyz = pin.PositionW;
    pout.PositionW.w = pin.ClipSpacePosZ;
 
    if (bUseLightmap)
    {
        float2 lightmapUV = pin.UV;
        
        lightmapUV.y = 1 - lightmapUV.y;
        lightmapUV = lightmapUV * cUVOffsetScale.xy + cUVOffsetScale.zw;
        lightmapUV.y = 1 - lightmapUV.y;
        
        pout.Light = gLightMapArray.Sample(gLinearWrap, float3(lightmapUV, cUVIndex));

        if (bUseDirectMap)
        {
            float4 direction = gDirectionArray.Sample(gPointWrap, float3(lightmapUV, cUVIndex));
            direction.xyz = direction.xyz * 2 - 1;
            direction.x = -direction.x;
            direction.z = -direction.z;
            float halfLambert = dot(resultNormal.xyz, direction.xyz) * 0.5 + 0.5;
            pout.Light = pout.Light * halfLambert / max(1e-4, direction.w);
        }
    }
    
    return pout;
#else
    
    // 라이팅 
    float3 directLighting = 0.0;
    
    Material material;
    material.albedo = resultAlbedo;
    //material.roughness = roughness;
    material.roughness = resultRoughness;
    //material.metallic = metalness;
    material.metallic = resultMetalic;
    float3 toEye = normalize(eyePosition - pin.PositionW);
    
    float shadowRatio = 1.f;
    // DirectionalLight 처리
    {
        for (uint i = 0; i < numOfDirectionalLight; ++i)
        {
            float3 currentDirectLighting = ComputeDirectionLight(directionalLights[i], material, toEye, resultNormal);
            
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
            directLighting += ComputePointLight(pointLights[i], material, toEye, resultNormal, pin.PositionW);
        }
    }
    
    // SpotLight 처리
    {
        for (uint i = 0; i < numOfSpotLight; ++i)
        {
            directLighting += ComputeSpotLight(spotLights[i], material, toEye, resultNormal, pin.PositionW);
        }
    }
    
    //float ndotv = max(0.f, dot(normal, normalize(toEye)));
    //float3 F0 = lerp(Fdielectric, albedo, metalness);
    //
    //// ibl
    //float3 ambientLighting = 0.f;
    //
    //if (isUseIBL)
    //{
    //    float3 irradiance = gDiffuseCubMap.Sample(gSamplerAnisotropic, normal).rgb;
    //    float3 F = fresnelSchlick(F0, ndotv);
    //    float3 kd = lerp(1.0 - F, 0.0, metalness);
    //    float3 diffuseIBL = kd * albedo * irradiance;
    //    
    //    float3 viewReflect = 2.0 * ndotv * normal - toEye;
    //    uint specularTextureLevels = QueryTextureLevels(gSpecularCubeMap);
    //    float3 specularIrradiance = gSpecularCubeMap.SampleLevel(gSamplerAnisotropic, viewReflect, roughness * specularTextureLevels).rgb;
    //    float2 specularBRDF = gSpecularBRDF_LUT.Sample(gSP_BRDF_LUT_Sampler, float2(ndotv, roughness)).rg;
    //    float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
    //
    //    ambientLighting = diffuseIBL + specularIBL;
    //}
   
    //result = pow(result, 1/2.2);
    
    pout.Color = float4(directLighting, 1.0f);
    return pout;
    //return float4(resultAlbedo, 1.0f);
    //return float4(shadowRatio, 0, 0, 1.0f);
#endif
}

