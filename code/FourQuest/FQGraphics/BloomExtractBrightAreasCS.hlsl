Texture2D gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);

SamplerState defaultSampler : register(s0);

cbuffer BloomParams : register(b0)
{
    float gThreshold;
}

float4 ExtractBrightAreas(float4 color, float threshold)
{
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    return (brightness > threshold) ? color : float4(0, 0, 0, 0);
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    gOutputTexture.GetDimensions(width, height);
    
    if (DTid.x < width && DTid.y < height)
    {
        float2 uv = DTid.xy / float2(width, height);
        float4 color = gInputTexture.SampleLevel(defaultSampler, uv, 0);
        gOutputTexture[DTid.xy] = ExtractBrightAreas(color, gThreshold);
    }
}