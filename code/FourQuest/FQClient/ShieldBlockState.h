#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class ShieldBlockState : public fq::game_module::IStateBehaviour
	{
	public:
		ShieldBlockState();
		~ShieldBlockState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};
} 