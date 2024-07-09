#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	/// <summary>
	/// 근접몬스터가 아무것도 하지 않는 상태 
	/// </summary>
	class MeleeMonsterIdleState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterIdleState();
		~MeleeMonsterIdleState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	};


}