#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class Player;

	class StaffSoulAttack : public fq::game_module::Component
	{
	public:
		StaffSoulAttack();
		~StaffSoulAttack();

		void SetPlayer(fq::client::Player* player);

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnUpdate(float dt) override;
		void emitAttack();

	private:
		std::shared_ptr<game_module::GameObject> mPlayerObject;
		Player* mPlayer;
		game_module::PrefabResource mAttack;

		float mAttackEmitElapsedTime;
		float mAttackEmitTick;
		float mAttackDuration;
		float mAttackElapsedTime;
		float mKnockBackPower;

		friend void RegisterMetaData();
	};

}
