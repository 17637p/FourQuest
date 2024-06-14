#include "ParticleCommon.hlsli"

cbuffer cbDeadList : register(b2)
{
    uint gNumDeadParticles;
    uint3 DeadListCount_pad;
};

SamplerState g_samWrapLinear : register(s0);
SamplerState g_samClampLinear : register(s1);

Texture2D gRandomTexture : register(t0); // ���� �ؽ�ó
RWStructuredBuffer<Particle> g_ParticleBuffer : register(u0); // ��ƼŬ ������
ConsumeStructuredBuffer<uint> g_DeadListToAllocFrom : register(u1); // ��� ������ �ε���

// SV_DispatchThreadID = dispatch * numthread + groupThreadId
[numthreads(1024, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x < gNumDeadParticles && id.x < gParticleInstance.NumToEmit)
    {
        Particle pa = (Particle) 0;
		
		// ���� ����
        float2 uv = float2(id.x / 1024.0, gParticleInstance.TimePos + gParticleInstance.RandomSeed);
        float4 randomValues0 = gRandomTexture.SampleLevel(g_samWrapLinear, uv, 0);
        float2 uv1 = float2((id.x + 1) / 1024.0, gParticleInstance.TimePos + gParticleInstance.RandomSeed);
        float4 randomValues1 = gRandomTexture.SampleLevel(g_samWrapLinear, uv1, 0);
        float2 uv2 = float2((id.x + 1) / 1024.0, gParticleInstance.TimePos + gParticleInstance.RandomSeed);
        float4 randomValues2 = gRandomTexture.SampleLevel(g_samWrapLinear, uv2, 0);

		// ��ƼŬ ������ �ʱ�ȭ
        processShape(randomValues0, pa.PositionW, pa.Velocity);
        pa.Size = lerp(gParticleMain.StartSize[0], gParticleMain.StartSize[1], saturate((randomValues1.x + 1) * 0.5f));
        pa.Rotation = lerp(gParticleMain.StartRotation[0], gParticleMain.StartRotation[1], saturate((randomValues1.y + 1) * 0.5f));
        pa.Color = lerp(gParticleMain.StartColor[0], gParticleMain.StartColor[1], saturate((randomValues1.z + 1) * 0.5f));
        pa.StartColor = pa.Color;
        pa.LifeTime = lerp(gParticleMain.StartLifeTime[0], gParticleMain.StartLifeTime[1], saturate((randomValues1.w + 1) * 0.5f));
        pa.Age = 0.f;
        pa.GravityModfier = lerp(gParticleMain.GravityModfier[0], gParticleMain.GravityModfier[1], saturate((randomValues2.x + 1) * 0.5f));
        pa.EmitterPosition = float3(gParticleInstance.Transform[3][0], gParticleInstance.Transform[3][1], gParticleInstance.Transform[3][2]);

		// ���� ��ƼŬ �ε����� �����ͼ� ���ۿ� ����
        uint index = g_DeadListToAllocFrom.Consume();
        g_ParticleBuffer[index] = pa;
    }
}