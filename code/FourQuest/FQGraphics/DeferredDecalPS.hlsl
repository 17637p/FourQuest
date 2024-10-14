#include "common.hlsli"
#include "DecalCommon.hlsli"

Texture2D gAlbedoMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gEmissiveMap : register(t2);

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
    float4 Normal : SV_Target1;
    float4 Emissive : SV_Target2;
};

PixelOut main(VertexOut pin) : SV_Target
{
    PixelOut pout = (PixelOut) 0;
    
    // 기하구조의 텍스처 좌표
    float2 posTex = pin.PosTemp.xy / pin.PosTemp.w;
    
    // 각도가 벗어낫는지 확인
    float3 normal = gSourceNormalTexture.Sample(gPointClmap, posTex).xyz;
    clip(dot(normalize(normal), pin.Orientation) - cos(gNormalThresholdInRadian));
    
    // 그려질 범위가 벗어나는지 확인
    float3 sampledPosW = gPositionWTexture.Sample(gPointClmap, posTex);
    float4 sampledPosL = mul(float4(sampledPosW, 1.f), gInvWorldMatrix);
    clip(0.5f - abs(sampledPosL.xyz));
    
    float2 sampledPosTex = mul(float4(sampledPosL.xz + 0.5f, 0, 1), gTexMatrix);
    
    pout.Albedo = gBaseColor;
    
    if (gUseAlbedoMap)
    {
        pout.Albedo *= gAlbedoMap.Sample(gSamplerAnisotropic, sampledPosTex, 0);
    }
  
    clip(pout.Albedo.a - gAlphaCutoff);
    
    if (gUseNormalMap)
    {
        float4 normalInTex = gNormalMap.Sample(gSamplerAnisotropic, sampledPosTex);
        float4 sourceNormal = gSourceNormalTexture.Sample(gSamplerAnisotropic, posTex);
        float4 sourceTangent = gSourceTangentTexture.Sample(gSamplerAnisotropic, posTex);
        
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
        pout.Emissive.rgb *= gEmissiveMap.Sample(gSamplerAnisotropic, sampledPosTex, 0).rgb;
    }
    
    return pout;
}