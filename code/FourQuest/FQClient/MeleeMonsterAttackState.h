#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// 근접 몬스터 공격 상태
	/// </summary>
	class MeleeMonsterAttackState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterAttackState();
		~MeleeMonsterAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};

}
