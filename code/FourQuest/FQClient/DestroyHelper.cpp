#include "DestroyHelper.h"

#include "../FQGameModule/Scene.h"

fq::client::DestroyHelper::DestroyHelper()
	: mDuration(1.f)
	, mElapsed(0.f)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::DestroyHelper::Clone(std::shared_ptr<Component> clone) const
{
	std::shared_ptr<DestroyHelper> cloneMonster = std::dynamic_pointer_cast<DestroyHelper>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<DestroyHelper>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::DestroyHelper::OnStart()
{
	mElapsed = 0.f;
}

void fq::client::DestroyHelper::OnUpdate(float dt)
{
	mElapsed += dt;

	if (mDuration < mElapsed)
	{
		GetScene()->DestroyGameObject(GetGameObject());
	}
}
