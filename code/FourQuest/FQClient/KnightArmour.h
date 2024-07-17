#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	/// <summary>
	/// ±â»ç °©¿Ê
	/// </summary>
	class KnightArmour : public fq::game_module::Component
	{
	public:
		KnightArmour();
		~KnightArmour();

		void EmitSwordAttack();
		void EmitShieldAttack();
		void EmitDashAttack();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
	};

}
