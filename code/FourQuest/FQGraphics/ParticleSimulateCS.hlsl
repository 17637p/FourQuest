#include "ParticleCommon.hlsli"

RWStructuredBuffer<Particle> gParticleBuffer : register(u0);
AppendStructuredBuffer<uint> gDeadList : register(u1); // �̹� �����ӿ� ���� ��ƼŬ
RWStructuredBuffer<uint> gIndexBuffer : register(u2); //����ִ� ���
RWBuffer<uint> gDrawArgs : register(u3); // �׸��� ȣ�� ����

Texture2D gDeathBuffer : register(t0);

// �� ������ �׷�� 256���� ������� ó��
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    const float3 vGravity = float3(0.0, -9.81, 0.0);
	
    // ���� �׸��� �μ� �ʱ�ȭ
    if (id.x == 0)
    {
        gDrawArgs[0] = 0;
        gDrawArgs[1] = 1;
        gDrawArgs[2] = 0;
        gDrawArgs[3] = 0;
        gDrawArgs[4] = 0;
    }
    
    GroupMemoryBarrierWithGroupSync();

    Particle pa = gParticleBuffer[id.x];
	
    if (pa.Age < pa.LifeTime)
    {
        pa.Age += gParticleInstance.FrameTime * gParticleMain.SimulationSpeed;
        
        float ratio = pa.Age / pa.LifeTime;
        
        processForceOverLifeTime(pa.Velocity, pa.Velocity);
        pa.Velocity += pa.GravityModfier * vGravity * gParticleInstance.FrameTime * gParticleMain.SimulationSpeed;
        
        processLimitVelocityOverLifeTime(ratio, pa.Velocity, pa.Velocity);
        pa.PositionW += pa.Velocity * gParticleInstance.FrameTime * gParticleMain.SimulationSpeed;
        
        processVelocityOverLifeTime(ratio, pa, pa);
        processRoationOverLifeTime(pa.Rotation, pa.Rotation);
        
        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        processColorOverLifeTime(ratio, color);
        pa.Color = pa.StartColor * color;
        
        processSizeOverLifeTime(pa.Size, ratio, pa.Size);
        
        if (pa.Age > pa.LifeTime)
        {
            pa.LifeTime = -1;
            gDeadList.Append(id.x);
        }
        else
        {
            uint index = gIndexBuffer.IncrementCounter();
            gIndexBuffer[index] = id.x;
			
            uint dstIdx = 0;
            InterlockedAdd(gDrawArgs[0], 1, dstIdx);
        }
    }
    
    gParticleBuffer[id.x] = pa;
}
