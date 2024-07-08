#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	/// <summary>
	/// 근접몬스터 순찰 상태
	/// </summary>
	class MeleeMonsterPatrolState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterPatrolState();
		~MeleeMonsterPatrolState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	};


}