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

	public:
		void SummonLivingArmour(ControllerID id);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		void setUI(bool isVisible);
	private:
		game_module::PrefabResource mLivingArmourPrefab;

		unsigned short mPlayerCount;

		friend void RegisterMetaData();
	};
}