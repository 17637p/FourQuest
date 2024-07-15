
cbuffer cbFixed
{
    static const float weights[9] =
    {
        0.013519569015984728,
        0.047662179108871855,
        0.11723004402070096,
        0.20116755999375591,
        0.240841295721373,
        0.20116755999375591,
        0.11723004402070096,
        0.047662179108871855,
        0.013519569015984728
    };
    static const int gBlurRadius = 4;
};

Texture2D gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256
#define CacheSize (N + 2 * gBlurRadius)
groupshared float4 gCache[CacheSize];

[numthreads(N, 1, 1)]
void main(int3 groupThreadID : SV_GroupThreadID // 그룹 스레드 인덱스 0 ~ 255
	, int3 dispatchThreadID : SV_DispatchThreadID) // dispatch * thread ID
{
    // 왼쪽 가장자리 처리
    if (groupThreadID.x < gBlurRadius)
    {
        int x  = max(dispatchThreadID.x - gBlurRadius, 0);
        gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)]; 
    }
    
    // 오른쪽 가장자리 처리
    if (groupThreadID.x >= N - gBlurRadius)
    {
        int x = min(dispatchThreadID.x + gBlurRadius, gInput.Length.x - 1);
        gCache[groupThreadID.x + 2 * gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
    }

    // 5 ~ textureSize - 5
    gCache[groupThreadID.x + gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy - 1)];

    GroupMemoryBarrierWithGroupSync();

    float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
    for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
    {
        uint k = groupThreadID.x + gBlurRadius + i;
        blurColor += weights[i + gBlurRadius] * gCache[k];
    }

    gOutput[dispatchThreadID.xy] = blurColor;
}
