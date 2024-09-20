#include "DebuffPoisonZone.h"

#include "Player.h"

namespace fq::client
{
	DebuffPoisonZone::DebuffPoisonZone()
		: mPoisonDamage()
		, mPoisonTurm()
		, mDurationTime()
	{
	}

	DebuffPoisonZone::~DebuffPoisonZone()
	{
	}

	std::shared_ptr<fq::game_module::Component> DebuffPoisonZone::Clone(std::shared_ptr<Component> clone) const
	{
		std::shared_ptr<DebuffPoisonZone> cloneAttack = std::dynamic_pointer_cast<DebuffPoisonZone>(clone);

		if (cloneAttack == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneAttack = game_module::ObjectPool::GetInstance()->Assign<DebuffPoisonZone>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneAttack = *this;
		}

		return cloneAttack;
	}
}
