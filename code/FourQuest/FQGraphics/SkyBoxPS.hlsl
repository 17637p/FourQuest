struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

TextureCube cubeMap : register(t0); 
SamplerState linearSampler : register(s0);

float4 main(VertexOut pin) : SV_Target
{
	float4 color = cubeMap.Sample(linearSampler, pin.PosL); 
	
	return color; 
	//return float4(1,0,0,1); 
}
