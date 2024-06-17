struct HullOut
{
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
    float2 BoundsY : TEXCOORD1;
};

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

float2 gTexScale = 50.0f;

struct PatchTess
{
    float EdgeTess[4]   : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

cbuffer cbModelTransform : register(b0)
{
    float4x4 cWorld;
    float4x4 cWorldInvTranspose;
};

cbuffer cbSceneTransform : register(b1)
{
    float4x4 cView;
    float4x4 cViewProj;
};

Texture2D gHeightMap : register(t0);
SamplerState samHeightmap : register(s0); 

[domain("quad")]
DomainOut main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
    DomainOut dout;
    
    dout.PositionW = lerp(
        lerp(quad[0].PositionW, quad[1].PositionW, uv.x),
        lerp(quad[2].PositionW, quad[3].PositionW, uv.x),
        uv.y);

    dout.UV = lerp(
        lerp(quad[0].UV, quad[1].UV, uv.x),
        lerp(quad[2].UV, quad[3].UV, uv.x),
        uv.y);

    dout.TiledTex = dout.UV * gTexScale;

    dout.PositionW.y = gHeightMap.SampleLevel(samHeightmap, dout.UV, 0).r;
    dout.PositionW.y = dout.PositionW.y + cWorld._42;

    dout.PositionH = mul(float4(dout.PositionW, 1.f), cViewProj);

    // temp
    dout.ClipSpacePosZ = dout.PositionH.w;
    dout.NormalW = lerp(
        lerp(quad[0].NormalW, quad[1].NormalW, uv.x),
        lerp(quad[2].NormalW, quad[3].NormalW, uv.x),
        uv.y);
    dout.TangentW = lerp(
            lerp(quad[0].TangentW, quad[1].TangentW, uv.x),
            lerp(quad[2].TangentW, quad[3].TangentW, uv.x),
            uv.y);

    return dout;
}