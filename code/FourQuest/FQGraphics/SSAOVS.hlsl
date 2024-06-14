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
    // ������ x���п��� ����ü �� ��� �������� ������ ����Ǿ� �ִ�. 
    vout.ToFarPlane = gFrustumCorners[vin.ToFarPlaneIndex.x].xyz;
    // �ؽ�ó ��ǥ�� �״�� �ȼ� ���̴��� �Ѱ��ش�. 
    vout.Tex = float2((vin.Position.x + 1) / 2.0f, -(vin.Position.y - 1) / 2.0f);

    return vout;
}