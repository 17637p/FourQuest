struct VertexIn
{
    float3 Position         : POSITION;
    float3 ToFarPlaneIndex  : NORMAL;
};

struct VertexOut
{
    float4 PositionH    : SV_POSITION;
    float3 ToFarPlane   : TEXCOORD0;
    float2 Tex          : TEXCOORD1;
};

cbuffer cbFrustumCorners
{
    float4 gFrustumCorners[4];
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    vout.PositionH = float4(vin.Position, 1.0f);
    // 법선의 x성분에는 절두체 먼 평면 꼭짓점의 색인이 저장되어 있다. 
    vout.ToFarPlane = gFrustumCorners[vin.ToFarPlaneIndex.x].xyz;
    // 텍스처 좌표는 그대로 픽셀 셰이더에 넘겨준다. 
    vout.Tex = float2((vin.Position.x + 1) / 2.0f, -(vin.Position.y - 1) / 2.0f);

    return vout;
}