struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

cbuffer cbPerFrame : register(b0)
{
	float4x4 viewRotationProj;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), viewRotationProj).xyww;

	vout.PosL = normalize(vin.PosL); 

	return vout;
}
