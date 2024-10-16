struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 uv : Texcoord;
};

Texture2D outLineBlur : register(t0); 

SamplerState defaultSS : register(s0);

float4 main(VertexOut pin) : SV_Target
{
	float3 result;

	result = outLineBlur.Sample(defaultSS, pin.uv);

	return float4(result, 1);
}
