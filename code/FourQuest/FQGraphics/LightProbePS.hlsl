#include "Probe.hlsli"

struct VertexOut
{
	float4 PositionH : SV_POSITION;
    float3 NormalW : NORMAL;
};

cbuffer cbParticleFrame : register(b0)
{
    LightProbe lightProbe;
};

float4 main(VertexOut pin) : SV_Target
{
	float3 irr = ShadeIrad(float4(pin.NormalW, 1.0f), lightProbe) * lightProbe.cIntensity;

	return float4(irr, 1.0f); 
}
