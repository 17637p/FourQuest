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
