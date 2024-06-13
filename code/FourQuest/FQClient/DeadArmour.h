#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class DeadArmour :	public game_module::Component
	{
		using ControllerID = unsigned int;

	public:
		DeadArmour();
		~DeadArmour();
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		fq::game_module::PrefabResource GetLivingArmour() const { return mLivingArmour; }
		void SetLivingArmour(fq::game_module::PrefabResource val) { mLivingArmour = val; }

		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

	public:
		void SummonLivingArmour(ControllerID id);

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::PrefabResource mLivingArmour;

		unsigned int mPlayerCount;
	};
}