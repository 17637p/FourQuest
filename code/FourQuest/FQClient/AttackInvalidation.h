#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	/// <summary>
	/// 투사체 공격 무효화 
	/// </summary>
	class AttackInvalidation : public fq::game_module::Component
	{
	public:
		AttackInvalidation();
		~AttackInvalidation();

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
		entt::meta_handle GetHandle() override { return *this; }
	};


}