#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"

namespace fq::game_module
{
	class CharacterController;
}

namespace fq::client
{
	class DeadArmour;

	class Soul :public game_module::Component
	{
	public:
		Soul();
		~Soul();
		
		void DestorySoul();
		fq::client::ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(fq::client::ESoulType type);
	
		void SetSoulColor();
		void SetSoulManager();
		void SetSoulHP();
		void SetSoulDeath();
		void UpdateSoulHP(float dt);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		void OnTriggerExit(const fq::game_module::Collision& collision) override;
		void OnUpdate(float dt) override;
		void OnLateUpdate(float dt) override;

		void checkOtherPlayer();

	private:
		game_module::CharacterController* mController;
		game_module::GameObject* mSoulManagerObject;
		std::vector<DeadArmour*> mSelectArmours;
		ESoulType mSoulType;

		float mHP;
		int mDeathCount;
		bool mbIsDistanceInPlayer;

		friend void RegisterMetaData();
	};

}
