#include "Common.hlsli"

struct Particle
{
    float3 PositionW;
    float Size;
    
    float3 Velocity;
    float Rotation;
    
    float4 StartColor;
    float4 Color;
    
    float3 EmitterPosition;
    float GravityModfier;

    float LifeTime;
    float Age;
};

struct ParticleInstance
{
    Matrix Transform;
    float TimePos;
    float FrameTime;
    int NumToEmit;
    float RandomSeed;
};

struct ParticleMain
{
    float4 StartColor[2];

    float2 StartLifeTime;
    float2 StartSpeed;

    float2 StartRotation;
    float2 StartSize;

    float2 GravityModfier;
    float SimulationSpeed;
    int MaxParticleSize;
};

struct ParticleShape
{
    Matrix Transform;
    float3 Postion;
    float pad0; // 4
    float3 Rotation;
    float pad1; // 4
    float3 Scale;
    float pad2; // 4
    
    int ShapeType;
    int ModeType;
    float Angle;
    float Radius;

    float DountRadius;
    float Arc;
    float Speed;
    float Spread;

    float RadiusThickness;
};

struct ParticleVelocityOverLifetime
{
    float3 Velocity; // 12
    float pad0; // 4
    float3 Orbital; // 12
    float pad1; // 4
    float3 Offset; // 12
    float pad2; // 4
    int bIsUsed; // 4
};

struct ParticleLimitVelocityOverLifetime
{
    float Speed;
    float Dampen; // 제한 속도 초과 시 감소 비율
    int bIsUsed;
};

struct ParticleForceOverLifeTime
{
    float3 Force;
    int bIsUsed;
};

struct ParticleColorOverLifetime
{
    float4 ColorRatios[8];
    float4 AlphaRatios[8];
    uint AlphaRatioCount;
    uint ColorRatioCount;
    int bIsUsed;
};

struct ParticleSizeOverLifetime
{
    float2 PointA;
    float2 PointB;
    float2 PointC;
    float2 PointD;
    int bIsUsed;
};

struct ParticleRotationOverLifetime
{
    float AngularVelocity;
    int bIsUsed;
};


cbuffer cbParticleFrame : register(b0)
{
    matrix gViewMatrix;
    matrix gProjMatrix;
    matrix gInvProjMatrix;
    uint gScreenWidth;
    uint gScreenHeight;
};
  
cbuffer cbParticleObject : register(b1)
{
    ParticleInstance gParticleInstance;
    
    ParticleMain gParticleMain;
    
    ParticleShape gParticleShape;
    
    ParticleVelocityOverLifetime gParticleVelocityOverLifetime;
    
    ParticleLimitVelocityOverLifetime gParticleLimitVelocityOverLifetime;

    ParticleForceOverLifeTime gParticleForceOverLifeTime;
    
    ParticleColorOverLifetime gParticleColorOverLifetime;
    
    ParticleSizeOverLifetime gParticleSizeOverLifetime;
    
    ParticleRotationOverLifetime gParticleRotationOverLifetime;

    ParticleMaterial gParticleRender;
};

#define SHAPE_SPHERE 0
#define SHAPE_HEMISPHERE 1
#define SHAPE_CONE 2
#define SHAPE_DONUT 3
#define SHAPE_BOX 4
#define SHAPE_CIRCLE 5
#define SHAPE_RECTANGLE 6

void processShape(float4 random, out float3 posW, out float3 velocity)
{
    float speed = lerp(gParticleMain.StartSpeed[0], gParticleMain.StartSpeed[1], (random.x + 1) * 0.5f);
    matrix finalTransform = mul(gParticleShape.Transform, gParticleInstance.Transform);

    posW = float3(gParticleInstance.Transform[3][0], gParticleInstance.Transform[3][1], gParticleInstance.Transform[3][2]); /*gCommonInfo.gEmitPosW.xyz;*/
    velocity = speed * normalize(float3(finalTransform[1][0], finalTransform[1][1], finalTransform[1][2])); /* gCommonInfo.gEmitDirW;*/
    
    if (gParticleShape.ShapeType == SHAPE_SPHERE)
    {
        float3 nRandom = normalize(random.xyz); // xz 평면 기준으로 Arc를 적용시키자.
        float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
        float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata
        float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
        _radian = _radian * (gParticleShape.Arc / (2 * PI)); // 변환.
        nRandom.x = cos(_radian) * _length;
        nRandom.z = sin(_radian) * _length;

        velocity = speed * nRandom;
        
        float posLength = length(nRandom * gParticleShape.Radius);
        float radiusThickness = saturate(gParticleShape.RadiusThickness);
        posLength = radiusThickness * posLength + (1.0f - radiusThickness) * saturate((random.w + 1.f) * 0.5f) * posLength;
        
        float3 pos = nRandom * posLength;
        posW = mul(float4(pos, 1), finalTransform).xyz;
    }
    else if (gParticleShape.ShapeType == SHAPE_HEMISPHERE)
    {
        float3 nRandom = normalize(random.xyz);
        float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
        float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata
        float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
         
        _radian = _radian * (gParticleShape.Arc / (2 * PI)); // 변환.
        nRandom.x = cos(_radian) * _length;
        nRandom.z = sin(_radian) * _length;

        velocity = speed * float3(nRandom.x, abs(nRandom.y), nRandom.z);
			
        float posLength = length(nRandom * gParticleShape.Radius);
        float radiusThickness = saturate(gParticleShape.RadiusThickness);
        posLength = radiusThickness * posLength + (1.0f - radiusThickness) * saturate((random.w + 1.f) * 0.5f) * posLength;
        
        float3 pos = nRandom * posLength;
        posW = mul(float4(pos.x, abs(pos.y), pos.z, 1), finalTransform);
    }
    else if (gParticleShape.ShapeType == SHAPE_CONE)
    {
        float a = 1 / cos(gParticleShape.Angle); // 빗변 높이가 1이라고 가정.
        float y = abs(sin(gParticleShape.Angle) * a); // 밑변의 길이.

        float TopRadius = (y + gParticleShape.Radius);
            
        TopRadius = TopRadius / gParticleShape.Radius; // 스케일 값으로 사용.
            
        float3 nRandom = normalize(random.xyz); // xz 평면 기준으로 Arc를 적용시키자.
            
        float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
            
        float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata

        float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
        _radian = _radian * (gParticleShape.Arc / (2 * PI)); // 변환.
            
        nRandom.x = cos(_radian) * _length;
        nRandom.z = sin(_radian) * _length;
            
        float posLength = length(nRandom.xz * gParticleShape.Radius);
        float radiusThickness = saturate(gParticleShape.RadiusThickness);
        posLength = radiusThickness * posLength + (1.0f - radiusThickness) * saturate((random.w + 1.f) * 0.5f) * posLength;
        
        float2 pos = normalize(nRandom.xz) * posLength;
            
        float3 topPosition = mul(float4((float3(pos.x, 0, pos.y) * TopRadius + float3(0, 1, 0)).xyz, 1), finalTransform).xyz;
        posW = mul(float4(float3(pos.x, 0, pos.y), 1), finalTransform).xyz;

        velocity = normalize(topPosition - posW) * speed;
    }
    else if (gParticleShape.ShapeType == SHAPE_DONUT)
    {
        float3 nRandom = normalize(random.xyz); // xz 평면 기준으로 Arc를 적용시키자.
        float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
        float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata
        float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
        _radian = _radian * (gParticleShape.Arc / (2 * PI)); // 변환.
        nRandom.x = cos(_radian) * _length;
        nRandom.z = sin(_radian) * _length;
            
        // 반지름 벡터
        float2 radiusVec = nRandom.xz;
        radiusVec = normalize(radiusVec);
        float posLength = length(random.xy * gParticleShape.DountRadius);
       // posLength = gParticleShape.RadiusThickness * gParticleShape.DountRadius + (1.0f - gParticleShape.RadiusThickness) * posLength;
        float radiusThickness = saturate(gParticleShape.RadiusThickness);
        posLength = radiusThickness * gParticleShape.DountRadius + (1.0f - radiusThickness) * saturate((random.w + 1.f) * 0.5f) * gParticleShape.DountRadius;
        
        float3 pos = float3(normalize(random.xy) * posLength, 0.0f);
            
        posW = mul(float4((float3(radiusVec.x, 0, radiusVec.y) * gParticleShape.Radius + pos).xyz, 1), finalTransform).xyz;
        velocity = speed * normalize(random.yzw);
    }
    else if (gParticleShape.ShapeType == SHAPE_BOX)
    {
        posW = mul(float4(random.xyz, 1), finalTransform);
    }
    else if (gParticleShape.ShapeType == SHAPE_CIRCLE)
    {
        float3 nRandom = normalize(random.xyz); // xz 평면 기준으로 Arc를 적용시키자.
        float _length = length(nRandom.xz); // 벡터의 길이를 구한다.
        float cosTheata = nRandom.xz.x / _length; // 벡터의 길이 = 빗변, \\  밑면 / 빗변 = cosTheata
        float _radian = nRandom.z > 0 ? acos(cosTheata) : 2 * PI - acos(cosTheata); // 파티클의 방출 각도. 
            
        _radian = _radian * (gParticleShape.Arc / (2 * PI)); // 변환.
        nRandom.x = cos(_radian) * _length;
        nRandom.z = sin(_radian) * _length;
            
        float2 radiusVec = nRandom.xz;
        float posLength = length(nRandom.xz * gParticleShape.Radius);
        //posLength = gParticleShape.RadiusThickness * gParticleShape.Radius + (1.0f - gParticleShape.RadiusThickness) * posLength;
        float radiusThickness = saturate(gParticleShape.RadiusThickness);
        posLength = radiusThickness * gParticleShape.Radius + (1.0f - radiusThickness) * saturate((random.w + 1.f) * 0.5f) * gParticleShape.Radius;
       
        float2 pos = normalize(nRandom.xz) * posLength;
        posW = mul(float4(pos.x, 0, pos.y, 1), finalTransform).xyz;
        velocity = normalize(mul(float3(nRandom.x, 0, nRandom.z), (float3x3) finalTransform)) * speed;
    }
    else if (gParticleShape.ShapeType == SHAPE_RECTANGLE)
    {
        posW = mul(float4((float3(random.x, 0, random.z)).xyz, 1), finalTransform);
    }
}

void processOrbital(float3 posW, float3 InitEmitterPos, float3 velW, out float3 velW_Out, out float3 posW_Out);

void processVelocityOverLifeTime(float ratio, Particle particleIn, out Particle particelOut)
{
    if (gParticleVelocityOverLifetime.bIsUsed)
    {
        float3 velocity = float3(0, 0, 0);
        
        velocity = lerp(0, gParticleVelocityOverLifetime.Velocity, ratio);

        particleIn.PositionW += velocity * gParticleInstance.FrameTime * gParticleMain.SimulationSpeed;;

        processOrbital(particleIn.PositionW, particleIn.EmitterPosition, particleIn.Velocity, particleIn.Velocity, particleIn.PositionW);
    }

    particelOut = particleIn;
}

void processLimitVelocityOverLifeTime(float ratio, float3 VelWIn, out float3 VelWOut)
{
    if (gParticleLimitVelocityOverLifetime.bIsUsed)
    {
        // float t = ratio;
        // float s = 1.0f - t;
        // 
        // float2 speed = pow(s, 3) * gParticleLifeTimeDependent.PointA
		// 	+ 3 * pow(s, 2) * t * gParticleLifeTimeDependent.PointB
		// 	+ 3 * s * pow(t, 2) * gParticleLifeTimeDependent.PointC
		// 	+ pow(t, 3) * gParticleLifeTimeDependent.PointD;
        // 
        // speed = clamp(speed, 0, 1);
        // speed.y *= gParticleLifeTimeDependent.Speed;
        
        float nowSpeed = length(VelWIn);
        
        if (nowSpeed > gParticleLimitVelocityOverLifetime.Speed)
        {
            float overSpeed = nowSpeed - gParticleLimitVelocityOverLifetime.Speed;
            nowSpeed -= overSpeed * gParticleLimitVelocityOverLifetime.Dampen;
            VelWIn = nowSpeed * normalize(VelWIn);
        }
    }

    VelWOut = VelWIn;
}

void processForceOverLifeTime(float3 velW, out float3 velWOut)
{
    if (gParticleForceOverLifeTime.bIsUsed)
    {
        float3 force = float3(0, 0, 0);

        velWOut = velW + gParticleForceOverLifeTime.Force * gParticleInstance.FrameTime * gParticleMain.SimulationSpeed;
    }
    else
    {
        velWOut = velW;
    }

}
void processColorOverLifeTime(float ratio, out float4 color)
{
    if (gParticleColorOverLifetime.bIsUsed)
    {
        
        uint i;
        
        for (i = 0; i < gParticleColorOverLifetime.AlphaRatioCount; ++i)
        {
            float curRatio = gParticleColorOverLifetime.AlphaRatios[i].y;
            
            if (curRatio > ratio)
            {
                break;
            }
        }
        
        if (i == gParticleColorOverLifetime.AlphaRatioCount)
        {
            --i;
        }
        
        float endAlpha = gParticleColorOverLifetime.AlphaRatios[i].x;
        float startAlpha = 0.f;
        float alphaRatio = 1.f;
                
        if (i != 0)
        {
            float curRatio = gParticleColorOverLifetime.AlphaRatios[i].y;
            float prevRatio = gParticleColorOverLifetime.AlphaRatios[i - 1].y;
            
            startAlpha = gParticleColorOverLifetime.AlphaRatios[i - 1].x;
            alphaRatio = (ratio - prevRatio) / (curRatio - prevRatio);
            alphaRatio = clamp(alphaRatio, 0, 1);
        }
        
        color.a = lerp(startAlpha, endAlpha, alphaRatio);
        
        for (i = 0; i < gParticleColorOverLifetime.ColorRatioCount; ++i)
        {
            float curRatio = gParticleColorOverLifetime.ColorRatios[i].w;
            
            if (curRatio > ratio)
            {
                break;
            }
        }
        
        if (i == gParticleColorOverLifetime.ColorRatioCount)
        {
            --i;
        }
        
        float3 endColor = gParticleColorOverLifetime.ColorRatios[i].xyz;
        float3 startColor = 0.f;
        float colorRatio = 1.f;
                
        if (i != 0)
        {
            float curRatio = gParticleColorOverLifetime.ColorRatios[i].w;
            float prevRatio = gParticleColorOverLifetime.ColorRatios[i - 1].w;
            
            startColor = gParticleColorOverLifetime.ColorRatios[i - 1].xyz;
            colorRatio = (ratio - prevRatio) / (curRatio - prevRatio);
            
            colorRatio = clamp(colorRatio, 0, 1);
        }
        
        color.xyz = lerp(startColor, endColor, colorRatio);
    }
    else
    {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

void processSizeOverLifeTime(float sizeIn, float3 ratio, out float sizeOut)
{
    if (gParticleSizeOverLifetime.bIsUsed)
    {
        float t = ratio;
        float s = 1.0f - t;

        float2 sizePos = pow(s, 3) * gParticleSizeOverLifetime.PointA
			+ 3 * pow(s, 2) * t * gParticleSizeOverLifetime.PointB
			+ 3 * s * pow(t, 2) * gParticleSizeOverLifetime.PointC
			+ pow(t, 3) * gParticleSizeOverLifetime.PointD;
        
        sizeOut = sizePos.x;
    }
    else
    {
        sizeOut = sizeIn;
    }
}
void processRoationOverLifeTime(float rotateIn, out float rotateOut)
{
    if (gParticleRotationOverLifetime.bIsUsed)
    {
        rotateOut = rotateIn + gParticleRotationOverLifetime.AngularVelocity * gParticleInstance.FrameTime * gParticleMain.SimulationSpeed;
    }
    else
    {
        rotateOut = rotateIn;
    }
}

float4 qmul(float4 q1, float4 q2)
{
    return float4(
        q2.xyz * q1.w + q1.xyz * q2.w + cross(q1.xyz, q2.xyz),
        q1.w * q2.w - dot(q1.xyz, q2.xyz)
    );
}

float3 rotate_vector(float3 v, float4 r)
{
    float4 r_c = r * float4(-1, -1, -1, 1);
    return qmul(r, qmul(float4(v, 0), r_c)).xyz;
}

float4 rotate_angle_axis(float angle, float3 axis)
{
    float sn = sin(angle * 0.5);
    float cs = cos(angle * 0.5);
    return float4(axis * sn, cs);
}

void processOrbital(float3 posW, float3 InitEmitterPos, float3 velW, out float3 velW_Out, out float3 posW_Out)
{
    InitEmitterPos += gParticleVelocityOverLifetime.Offset;
    float3 axis = gParticleVelocityOverLifetime.Orbital;

    if (length(axis) != 0)
    {
        float3 n_axis = normalize(axis);

        float3 orbitalCenterPos = dot(posW, n_axis) * n_axis + InitEmitterPos; // 

        float3 offset = posW - InitEmitterPos; // 

        float power = length(axis);
        
        float angle = gParticleInstance.FrameTime * power * gParticleMain.SimulationSpeed;;

        float4 rotateQuat = rotate_angle_axis(angle, n_axis);

        float3 rotated = rotate_vector(offset, rotateQuat);

        velW = rotate_vector(velW, rotateQuat);

        posW = rotated + InitEmitterPos;
    }
    
    velW_Out = velW;
    posW_Out = posW;

}
