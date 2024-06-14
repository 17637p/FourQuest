#include "ParticleCommon.hlsli"

struct VertexOut
{
    float3 PosW : POSITION;
    float4 Color : COLOR;
    float4 Age_LifeTime_Rotation_Size : TEXCOORD0;
};

StructuredBuffer<Particle> gParticleBuffer : register(t0);
StructuredBuffer<uint> gIndexBuffer : register(t1);

VertexOut main(uint VertexId : SV_VertexID)
{
    VertexOut output = (VertexOut) 0;

    uint index = gIndexBuffer[VertexId];
    Particle pa = gParticleBuffer[index];
	
    output.PosW = pa.PositionW;
    output.Color = pa.Color;
    output.Age_LifeTime_Rotation_Size = float4(pa.Age, pa.LifeTime, pa.Rotation, pa.Size);
    
    return output;
}