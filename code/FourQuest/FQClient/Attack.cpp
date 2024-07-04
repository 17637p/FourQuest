#include "Attack.h"

std::shared_ptr<fq::game_module::Component> fq::client::Attack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<Attack> cloneAttack = std::dynamic_pointer_cast<Attack>(clone);

	if (cloneAttack == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAttack = game_module::ObjectPool::GetInstance()->Assign<Attack>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAttack = *this;
	}

	return cloneAttack;
}

fq::client::Attack::Attack()
	:mAttackPower(-1)
	,mElapsedTime(0.f)
	,mAttackTime(0.1f)
	,mAttackDirection(NoDirection)
{

}

fq::client::Attack::~Attack()
{

}

void fq::client::Attack::OnUpdate(float dt)
{
	mElapsedTime += dt;

	if (mElapsedTime >= mAttackTime)
	{
		GetScene()->DestroyGameObject(GetGameObject());
	}
}
