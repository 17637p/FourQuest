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
	, mDestroyElapsedTime(0.f)
	, mDestroyTime(0.1f)
	, mAttackDirection(NoDirection)
	, mRemainingAttackCount(1)
	, mbIsInfinite(true)
	, mPushPower(0.f)
{}

fq::client::Attack::~Attack()
{

}

void fq::client::Attack::OnUpdate(float dt)
{
	mDestroyElapsedTime += dt;

	if (mDestroyElapsedTime >= mDestroyTime)
	{
		GetScene()->DestroyGameObject(GetGameObject());
	}
}


bool fq::client::Attack::ProcessAttack() 
{
	// 삭제된 공격은 처리하지 않습니다 
	if (GetGameObject()->IsDestroyed())
	{
		return false;
	}

	if (!mbIsInfinite)
	{
		mRemainingAttackCount--;

		if (mRemainingAttackCount == 0)
		{
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}

	return true;
}
