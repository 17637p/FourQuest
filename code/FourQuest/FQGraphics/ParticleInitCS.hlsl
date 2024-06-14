#include "ParticleCommon.hlsli"

RWStructuredBuffer<Particle> gParticleBuffer : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    gParticleBuffer[id.x] = (Particle) 0;
}