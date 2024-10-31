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

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<DestroyHelper>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
