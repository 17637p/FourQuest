Texture2D gDownSampleTexture : register(t0);
Texture2D gInputTexture : register(t1);
RWTexture2D<float4> gOutputTexture : register(u0);

SamplerState linearSampler : register(s0);

cbuffer BloomParams : register(b0)
{
    float gThreshold;
    float gScatter;
    bool bUseScatter;
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    gOutputTexture.GetDimensions(width, height);
    
    if (DTid.x < width && DTid.y < height)
    {
        float2 uv = DTid.xy / float2(width, height);
        float4 downSampleColor = gDownSampleTexture.SampleLevel(linearSampler, uv, 0);
        float4 inputColor = gInputTexture.SampleLevel(linearSampler, uv, 0);
        
        if (bUseScatter)
        {
            gOutputTexture[DTid.xy] = downSampleColor * gScatter + inputColor;
        }
        else
        {
            gOutputTexture[DTid.xy] = downSampleColor + inputColor;
        }
    }
}