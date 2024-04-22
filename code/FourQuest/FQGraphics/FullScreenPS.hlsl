Texture2D gOffScreenMap : register(t0);
SamplerState gSamplerPoint : register(s1);

float4 main(float2 uv : Texcoord) : SV_TARGET
{
    float3 color = gOffScreenMap.Sample(gSamplerPoint, uv).rgb;
    
    return float4(color, 1.0);
}