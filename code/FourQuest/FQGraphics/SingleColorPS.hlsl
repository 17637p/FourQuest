struct VertexOut
{
	float4 PositionH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(VertexOut pin) : SV_Target
{
	return pin.Color; 
}