Texture2D gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);

SamplerState linearSampler : register(s0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    gOutputTexture.GetDimensions(width, height);
    
    if (DTid.x < width && DTid.y < height)
    {
        float2 uv = DTid.xy / float2(width, height);
        float4 color = gInputTexture.SampleLevel(linearSampler, uv, 0);
        gOutputTexture[DTid.xy] = color;
    }
}