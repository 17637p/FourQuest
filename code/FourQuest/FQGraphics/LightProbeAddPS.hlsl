struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 uv : Texcoord;
};

Texture2D OffScreen : register(t0); 
Texture2D lightProbe : register(t1); 

SamplerState defaultSS : register(s0);

float4 main(VertexOut pin) : SV_Target
{
	float3 result;

	result = OffScreen.Sample(defaultSS, pin.uv);
	result += lightProbe.Sample(defaultSS, pin.uv);

	return float4(result, 1);
}
