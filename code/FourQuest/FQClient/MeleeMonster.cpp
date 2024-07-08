#include "MeleeMonster.h"

fq::client::MeleeMonster::MeleeMonster()
	:mMaxHp(0.f)
	,mHp(100.f)
	,mAttackPower(10.f)
	,mMoveSpeed(1.f)
	,mAcceleration(1.f)
	,mAttackRange(10.f)
	,mGameManager(nullptr)
	,mAnimator(nullptr)
	,mTarget(nullptr)
{}

fq::client::MeleeMonster::~MeleeMonster()
{}

std::shared_ptr<fq::game_module::Component> fq::client::MeleeMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MeleeMonster> cloneMonster = std::dynamic_pointer_cast<MeleeMonster>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MeleeMonster>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::MeleeMonster::SetTarget(game_module::GameObject* target)
{

}

void fq::client::MeleeMonster::OnStart()
{

}

void fq::client::MeleeMonster::EmitAttack()
{

}

void fq::client::MeleeMonster::LookAtTarget()
{

}
