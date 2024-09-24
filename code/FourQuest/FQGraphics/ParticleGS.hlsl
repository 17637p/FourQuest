#include "ParticleCommon.hlsli"

struct VS_OUTPUT
{
    float3 PosW : POSITION;
    float4 Color : COLOR;
    float4 Age_LifeTime_Rotation_Size : TEXCOORD0;
};

struct PS_INPUT
{
    float4 PosH : SV_Position;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
    float4 PosTex : TEXCOORD1;
};

static const matrix texTransform =
{
    0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f
};

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<PS_INPUT> SpriteStream)
{
    PS_INPUT output = (PS_INPUT) 0;

    const float2 offsets[4] =
    {
        float2(-1, 1),
		float2(1, 1),
		float2(-1, -1),
		float2(1, -1),
    };
    
    float2 UVs[4] =
    {
        float2(0, 0),
		float2(1, 0),
		float2(0, 1),
		float2(1, 1),
    };
    
    if (gParticleSprite.bIsUsed)
    {
        float age = input[0].Age_LifeTime_Rotation_Size.x;
        uint currIndex = age / gParticleSprite.FrameSecond;
        
        if (gParticleSprite.bIsLooping)
        {
            currIndex %= gParticleSprite.MaxFrameCount;
        }
        else
        {
            currIndex = min(currIndex, gParticleSprite.MaxFrameCount);
        }
        
        uint widthIndex = currIndex % gParticleSprite.WidthCount;
        uint heightindex = (currIndex / gParticleSprite.WidthCount) % gParticleSprite.HeightCount;
        float2 textureUVSize = float2(1 / (float) gParticleSprite.WidthCount, 1 / (float) gParticleSprite.HeightCount);
        
        UVs[0] = textureUVSize * float2(widthIndex, heightindex);
        UVs[1] = textureUVSize * float2(widthIndex + 1, heightindex);
        UVs[2] = textureUVSize * float2(widthIndex, heightindex + 1);
        UVs[3] = textureUVSize * float2(widthIndex + 1, heightindex + 1);
    }
    
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        float2 offset = offsets[i];
        float2 uv = UVs[i];
        
        float radius = input[0].Age_LifeTime_Rotation_Size.w;
        
        float s, c;
        sincos(input[0].Age_LifeTime_Rotation_Size.z, s, c);
        float2x2 rotation = { float2(c, -s), float2(s, c) };
		
        offset = mul(offset, rotation);

        output.PosH = mul(float4(input[0].PosW, 1.f), gViewMatrix);
        output.PosH.xy += radius * offset;
        output.PosH = mul(output.PosH, gProjMatrix);
        
        output.PosTex = mul(output.PosH, texTransform);
        
        output.Tex = mul(float4(uv, 0, 1), gParticleRender.TexTransform);
        output.Color = input[0].Color;

        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();
}