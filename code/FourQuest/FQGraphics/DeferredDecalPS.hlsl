#include "common.hlsli"
#include "DecalCommon.hlsli"

Texture2D gAlbedoMap : register(t0);
Texture2D gMetalnessMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gNormalMap : register(t3);
Texture2D gEmissiveMap : register(t4);

Texture2D gPositionWTexture : register(t5);
Texture2D gNormalTexture : register(t6);
Texture2D gSourceNormalTexture : register(t7);
Texture2D gSourceTangentTexture : register(t8);

SamplerState gLinearWrap : register(s0);
SamplerState gPointClmap : register(s1);
SamplerState gSamplerAnisotropic : register(s2); //	D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP

struct PixelOut
{
    float4 Albedo : SV_Target0;
    float Metalness : SV_Target1;
    float Roughness : SV_Target2;
    float4 Normal : SV_Target3;
    float4 Emissive : SV_Target4;
};

PixelOut main(VertexOut pin) : SV_Target
{
    PixelOut pout = (PixelOut) 0;
  
    float2 screenPosition = pin.PosClip.xy / pin.PosClip.w;
    float2 depthUV = screenPosition * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    
    float3 normal = gSourceNormalTexture.Sample(gSamplerAnisotropic, depthUV).xyz;
    clip(dot(normalize(normal), pin.Orientation) - cos(gNormalThresholdInRadian));
    
    float3 posW = gPositionWTexture.Sample(gSamplerAnisotropic, depthUV);
    float3 posV = mul(float4(posW, 1.f), gViewMatrix);

    if (posV.z == 0.f)
    {
        clip(-1);
    }
    
    float4 posLocalInTex = mul(float4(posV, 1.f), gInvWVMatrix);
    clip(0.5f - abs(posLocalInTex.xyz));
    
    float2 uv = posLocalInTex.xz;
    uv += 0.5f;
    
    uv = mul(float4(uv, 0, 1), gTexMatrix);
    
    pout.Albedo = gBaseColor;
    
    if (gUseAlbedoMap)
    {
        pout.Albedo *= gAlbedoMap.Sample(gSamplerAnisotropic, uv);
    }
  
    clip(pout.Albedo.a - gAlphaCutoff);
    
    if (gUseMetalnessMap)
    {
        pout.Metalness = gMetalnessMap.Sample(gSamplerAnisotropic, uv).x;
    }
    else
    {
        pout.Metalness = 0;
    }
    
    if (gUseRoughnessMap)
    {
        pout.Roughness = gRoughnessMap.Sample(gSamplerAnisotropic, uv).x;
    }
    else
    {
        pout.Roughness = 0;
    }
    
    if (gUseNormalMap)
    {
        float4 normalInTex = gNormalMap.Sample(gSamplerAnisotropic, uv);
        float4 sourceNormal = gSourceNormalTexture.Sample(gSamplerAnisotropic, depthUV);
        
        if (sourceNormal.x > 100.f)
        {
            clip(-1.f);
        }
        
        float4 sourceTangent = normalize(gSourceTangentTexture.Sample(gSamplerAnisotropic, depthUV) * 2 - 1);
        pout.Normal.xyz = normalize(NormalSampleToWorldSpace(normalInTex.xyz, sourceNormal.xyz, sourceTangent.xyz));
        pout.Normal.w = gNormalBlend;

    }
    else
    {
        pout.Normal = float4(0, 0, 0, 0);
    }
    
    pout.Emissive.rgb = gEmissiveColor.rgb;
    
    if (gUseEmissiveMap)
    {
        pout.Emissive.rgb *= gEmissiveMap.Sample(gSamplerAnisotropic, uv).rgb;
    }
    else
    {
        pout.Emissive = float4(0, 0, 0, 0);
    }
    
    return pout;
}