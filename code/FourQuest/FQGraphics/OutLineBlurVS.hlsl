struct VertexIn
{
	float2 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 uv : Texcoord;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

    vout.PosH = float4(vin.PosL, 0.0f, 1.0f);
	vout.uv = float2((vout.PosH.x + 1) / 2.0f, -(vout.PosH.y - 1) / 2.0f);

	return vout;
}
