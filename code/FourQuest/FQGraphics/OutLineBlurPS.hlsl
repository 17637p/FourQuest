struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 uv : Texcoord;
};

Texture2D outLine : register(t0); 
Texture2D offScreen : register(t1); 

SamplerState defaultSS : register(s0);

float4 main(VertexOut pin) : SV_Target
{
	float4 result = float4(0, 0, 0, 1);

	result += outLine.SampleLevel(defaultSS, pin.uv, 0);
	result += outLine.SampleLevel(defaultSS, pin.uv, 1);
	result += outLine.SampleLevel(defaultSS, pin.uv, 2);
	result += outLine.SampleLevel(defaultSS, pin.uv, 3);
	result += outLine.SampleLevel(defaultSS, pin.uv, 4);

	result += offScreen.Sample(defaultSS, pin.uv);

	return result;
}
