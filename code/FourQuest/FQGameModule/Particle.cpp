#include "Particle.h"

fq::game_module::Particle::Particle()
	: mParticleInfomation{}
	, mParticleObject(nullptr)
{}

fq::game_module::Particle::~Particle()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::Particle::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneParticle = std::dynamic_pointer_cast<Particle>(clone);

	if (cloneParticle == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneParticle = ObjectPool::GetInstance()->Assign<Particle>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneParticle = *this;
	}

	cloneParticle->mParticleObject = nullptr;

	return cloneParticle;
}
