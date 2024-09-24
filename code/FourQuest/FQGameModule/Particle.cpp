#define NOMINMAX
#include "Particle.h"

#include <algorithm>

#include "Transform.h"

fq::game_module::Particle::Particle()
	: mParticleInfo{}
	, mParticleObject(nullptr)
	, mParticleMaterial(nullptr)
{}

fq::game_module::Particle::~Particle()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::Particle::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneParticle = std::dynamic_pointer_cast<Particle>(clone);

	if (cloneParticle == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneParticle = ObjectPool::GetInstance()->Assign<Particle>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneParticle = *this;
	}

	cloneParticle->mParticleObject = nullptr;

	return cloneParticle;
}

void fq::game_module::Particle::SetParticleInfomation(fq::graphics::ParticleInfo val)
{
	mParticleInfo = val;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetMainParticleInfo(ParticleInfo::Main mainInfo)
{
	mParticleInfo.MainData = mainInfo;

	mParticleInfo.MainData.MaxParticleCount = std::min(mainInfo.MaxParticleCount, static_cast<size_t>(ParticleInfo::MAX_PARTICLE_COUNT));

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetEmissionInfo(ParticleInfo::Emission data)
{
	mParticleInfo.EmissionData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetShapeInfo(ParticleInfo::Shape data)
{
	mParticleInfo.ShapeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetVelocityOverLifetimeData(ParticleInfo::VelocityOverLifetime data)
{
	mParticleInfo.VelocityOverLifetimeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetLimitVelocityOverLifeTimeData(ParticleInfo::LimitVelocityOverLifetime data)
{
	mParticleInfo.LimitVelocityOverLifetimeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetForceOverLifetimeData(ParticleInfo::ForceOverLifetime data)
{
	mParticleInfo.ForceOverLifeTimeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetColorOverLifeTime(ParticleInfo::ColorOverLifetime data)
{
	mParticleInfo.ColorOverLifetimeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetSizeOverLifetime(ParticleInfo::SizeOverLifetime data)
{
	mParticleInfo.SizeOverLifetimeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetRotationOverLifetimeData(ParticleInfo::RotationOverLifetime data)
{
	mParticleInfo.RotationOverLifetimeData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetRenderData(ParticleInfo::Render data)
{
	mParticleInfo.RenderData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetInstanceData(ParticleInfo::Instance data)
{
	mParticleInfo.InstanceData = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetEmissionBursts(std::vector<ParticleInfo::Emission::Burst> bursts)
{
	mParticleInfo.EmissionData.Bursts = bursts;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetParticlesPerSecond(float data)
{
	mParticleInfo.EmissionData.ParticlesPerSecond = data;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::SetParticleMaterialInfo(graphics::ParticleMaterialInfo data)
{
	mParticleMaterialInfo = data;

	if (mParticleMaterial != nullptr)
	{
		mParticleMaterial->SetInfo(mParticleMaterialInfo);
	}
}

void fq::game_module::Particle::SetSpriteData(const ParticleInfo::Sprite& spriteData)
{
	mParticleInfo.SpriteData = spriteData;

	if (mParticleObject)
		mParticleObject->SetInfo(mParticleInfo);
}

void fq::game_module::Particle::OnUpdate(float dt)
{
	if (mParticleObject)
	{
		mParticleObject->SetTransform(GetComponent<Transform>()->GetWorldMatrix());
		mParticleObject->SetFrameTime(dt);
	}
}

void fq::game_module::Particle::OnStart()
{
}
