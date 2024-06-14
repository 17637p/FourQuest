struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 uv : Texcoord;
};

Texture2D singleColor : register(t0); 

SamplerState defaultSS : register(s0);

cbuffer cbScreenSize : register(b0)
{
    int screenWidth;
	int screenHeight;
}

float4 main(VertexOut pin) : SV_Target
{
	float mask[9] = 
	{
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1
	}; // Laplacian Filter

	float coord[3] = {-1, 0, 1};
	float4 c0 = 0;
	
	float xStep = 1.0f / screenWidth;
	float yStep = 1.0f / screenHeight;

	for(int i = 0; i < 9; i++)
	{
		c0 += mask[i] * singleColor.Sample(defaultSS, pin.uv + float2(coord[i%3] * xStep, coord[i/3] * yStep));
	}
	
	return c0 / 4;
	//return float4(1, 1, 1, 1);
}
