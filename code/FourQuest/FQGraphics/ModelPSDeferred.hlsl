#include "Common.hlsli"

#ifdef GEOMETRY

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
#ifdef STATIC
    float2 UV1 : TEXCOORD6;
#endif
};

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

cbuffer cbMaterial : register(b0)
{
    ModelMaterial gModelMaterial;
};

#ifdef STATIC
cbuffer cbLightmapInformation : register(b1)
{
    float4 cUVOffsetScale;
    uint cUVIndex;
    bool bUseDirectMap;
};
#endif

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gNormalMap : register(t3);
Texture2D gEmissiveMap : register(t4);
Texture2D gMetalnessSmoothness : register(t5);

Texture2DArray gLightMapArray : register(t6);
Texture2DArray gDirectionArray : register(t7);

SamplerState gSamplerAnisotropic : register(s0); //	D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP
SamplerState gPointWrap : register(s1); //	D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP

PixelOut main(VertexOut pin) : SV_TARGET
{
    PixelOut pout;

    pout.Albedo = gModelMaterial.BaseColor;

    if (gModelMaterial.UseAlbedoMap)
    {
        pout.Albedo *= pow(gAlbedoMap.Sample(gSamplerAnisotropic, pin.UV), 2.2f);
    }
    
    clip(pout.Albedo.a - gModelMaterial.AlphaCutoff);

    if (gModelMaterial.UseMetalnessMap)
    {
        pout.MetalnessRoughness.x = gMetalnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }
    else
    {
        pout.MetalnessRoughness.x = gModelMaterial.Metalness;
    }

    if (gModelMaterial.UseRoughnessMap)
    {
        pout.MetalnessRoughness.y = gRoughnessMap.Sample(gSamplerAnisotropic, pin.UV).r;
    }
    else
    {
        pout.MetalnessRoughness.y = gModelMaterial.Roughness;
    }

    if (gModelMaterial.UseNormalMap)
    {
        pout.Normal = gNormalMap.Sample(gSamplerAnisotropic, pin.UV);
        pout.Normal.xyz = normalize(NormalSampleToWorldSpace(pout.Normal.xyz, pin.NormalW, pin.TangentW));
    }
    else
    {
        pout.Normal.xyz = normalize(pin.NormalW);
    }

    if (gModelMaterial.UseMetalnessSmoothness)
    {
        float2 metalnessSmoothness = gMetalnessSmoothness.Sample(gSamplerAnisotropic, pin.UV).xy;
        pout.MetalnessRoughness.x = metalnessSmoothness.x;
        pout.MetalnessRoughness.y = 1 - metalnessSmoothness.y;
    }

    pout.SourceNormal.xyz = pin.NormalW.xyz;
    pout.SourceTangent.xyz = pin.TangentW.xyz;

    pout.PositionW.xyz = pin.PositionW;
    pout.PositionW.w = pin.ClipSpacePosZ;

    pout.Emissive.rgb = gModelMaterial.EmissiveColor.rgb;
    pout.Emissive.a = gModelMaterial.EmissiveIntensity / 255;

    if (gModelMaterial.UseEmissiveMap)
    {
        pout.Emissive.rgb *= gEmissiveMap.Sample(gSamplerAnisotropic, pin.UV).rgb;
    }

#ifdef STATIC
    // 현재 레거시로 라이트맵 적용, 추후에 PBR 방식으로 GI 연산 수정 예정
    pout.Light = gLightMapArray.Sample(gSamplerAnisotropic, float3(pin.UV1, cUVIndex));

   if (bUseDirectMap)
   {
       float4 direction = gDirectionArray.Sample(gPointWrap, float3(pin.UV1, cUVIndex));
       direction.xyz = direction.xyz * 2 - 1;
       direction.x = -direction.x;
       direction.z = -direction.z;
       float halfLambert = dot(pout.Normal.xyz, direction.xyz) * 0.5 + 0.5;
       pout.Light = pout.Light * halfLambert / max(1e-4, direction.w);
   }
#endif 
    return pout;
}

#elif defined SHADING

struct VertexOut
{
    float2 uv : Texcoord;
    float4 pos : SV_Position;
};

cbuffer cbDirectionalShadow : register(b0)
{
    matrix cLightViewProj[CascadeCount * MaxDirectionalShadowCount];
    float4 cCascadeEnds[CascadeCount];
    int cShadowCount;
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

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessRoughnessMap : register(t1);
Texture2D gNormalMap : register(t2);
Texture2D gEmissiveMap : register(t3);
Texture2D gPositionWorldMap : register(t4);
Texture2D gPreCalculatedLightMap : register(t5);
TextureCube gDiffuseCubMap : register(t6);
TextureCube gSpecularCubeMap : register(t7);
Texture2D gSpecularBRDF_LUT : register(t8);

Texture2DArray gDirectionalShadowMap : register(t9);
SamplerState gSamplerAnisotropic : register(s0); //	D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP
SamplerState gSP_BRDF_LUT_Sampler : register(s1); // D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP
SamplerState gPointClampSampler : register(s2);
SamplerComparisonState gShadowSampler : register(s3);

float4 main(VertexOut pin) : SV_TARGET
{
    float3 albedo = gAlbedoMap.Sample(gPointClampSampler, pin.uv).xyz;
    float4 sampledEmissive = gEmissiveMap.Sample(gPointClampSampler, pin.uv);
    float3 emissive = sampledEmissive.rgb * sampledEmissive.a * 255;
    float3 normal = gNormalMap.Sample(gPointClampSampler, pin.uv).xyz;

    if (normal.x > 100.f)
    {
        return float4(albedo + emissive, 1.f);
    }

    normal = normalize(normal);
    float2 metalnessRoughness = gMetalnessRoughnessMap.Sample(gPointClampSampler, pin.uv).xy;
    float4 position = gPositionWorldMap.Sample(gPointClampSampler, pin.uv);
    float3 positionW = position.xyz;
    float clipSpacePosZ = position.w;
    float3 toEye = normalize(eyePosition - positionW);
    float ndotv = max(0.f, dot(normal, normalize(toEye)));
    float3 F0 = lerp(Fdielectric, albedo, metalnessRoughness.x);
    
    float3 directLighting = 0.0;
    Material material;
    material.albedo = albedo;
    material.roughness = metalnessRoughness.y;
    material.metallic = metalnessRoughness.x;
    
    // DirectionalLight 처리
    for (uint i = 0; i < numOfDirectionalLight; ++i)
    {
        float3 currentDirectLighting = ComputeDirectionLight(directionalLights[i], material, toEye, normal);
      
        if (i < cShadowCount)
        {
            float shadowRatio = 1.f;
    
            uint index = 2;
    
            for (uint j = 0; j < 3; ++j)
            {
                if (clipSpacePosZ <= cCascadeEnds[i][j])
                {
                    index = j;
                    break;
                }
            }
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
            index = 2;
            uint shadowIndex = i * CascadeCount + index;
            float4 shadowPos = mul(float4(positionW, 1.f), cLightViewProj[shadowIndex]);
            shadowPos.x = shadowPos.x * 0.5f + 0.5f;
            shadowPos.y = shadowPos.y * -0.5f + 0.5f;
            shadowRatio = CalculateCascadeShadowRatio(gShadowSampler, gDirectionalShadowMap, shadowPos, shadowIndex, ShadowMapWidth);
            
            currentDirectLighting *= shadowRatio;
        }

        directLighting += currentDirectLighting;
    }
    
    // PointLight 처리
    for (uint i = 0; i < numOfPointLight; ++i)
    {
        directLighting += ComputePointLight(pointLights[i], material, toEye, normal, positionW);
    }
    
    // SpotLight 처리
    for (uint i = 0; i < numOfSpotLight; ++i)
    {
        directLighting += ComputeSpotLight(spotLights[i], material, toEye, normal, positionW);
    }
    
    // ibl
    float3 ambientLighting = 0.f;
    
    if (isUseIBL)
    {
        float3 irradiance = gDiffuseCubMap.Sample(gSamplerAnisotropic, normal).rgb;
        float3 F = fresnelSchlick(F0, ndotv);
        float3 kd = lerp(1.0 - F, 0.0, metalnessRoughness.x);
        float3 diffuseIBL = kd * albedo * irradiance;
        
        float3 viewReflect = 2.0 * ndotv * normal - toEye;
        uint specularTextureLevels = QueryTextureLevels(gSpecularCubeMap);
        float3 specularIrradiance = gSpecularCubeMap.SampleLevel(gSamplerAnisotropic, viewReflect, metalnessRoughness.y * specularTextureLevels).rgb;
        float2 specularBRDF = gSpecularBRDF_LUT.Sample(gSP_BRDF_LUT_Sampler, float2(ndotv, metalnessRoughness.y)).rg;
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        ambientLighting = diffuseIBL + specularIBL;
    }
     
    float3 preCaculatedLight = albedo * gPreCalculatedLightMap.Sample(gPointClampSampler, pin.uv).xyz;

    return float4(directLighting + ambientLighting + emissive + preCaculatedLight, 1.f);
}

#endif
