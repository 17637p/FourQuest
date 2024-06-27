#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class PlayerDeadState : public game_module::IStateBehaviour
	{
	public:
		PlayerDeadState();
		~PlayerDeadState();

		std::shared_ptr<IStateBehaviour> Clone() override;


	private:
		entt::meta_handle GetHandle() override { return *this; }
	};


}