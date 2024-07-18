#include "Attack.h"

std::shared_ptr<fq::game_module::Component> fq::client::Attack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<Attack> cloneAttack = std::dynamic_pointer_cast<Attack>(clone);

	if (cloneAttack == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAttack = game_module::ObjectPool::GetInstance()->Assign<Attack>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
	// ������ ������ ó������ �ʽ��ϴ� 
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
