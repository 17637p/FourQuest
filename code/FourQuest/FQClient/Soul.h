#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"
#include "BGaugeUI.h"

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
		void SetSoulHP();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		void OnTriggerExit(const fq::game_module::Collision& collision) override;
		void OnUpdate(float dt) override;
		void OnLateUpdate(float dt) override;

		// 영혼 주변 갑옷 플레이어 검색 및 사정거리 안에 있으면 버프
		void checkOtherPlayer();
		// 영혼 인근 갑옷 상호작용
		void selectArmour();
		// 영혼 HP 업데이트
		void updateSoulHP(float dt);

	private:
		game_module::CharacterController* mController;
		std::vector<DeadArmour*> mSelectArmours;
		BGaugeUI* mBGaugeUI;
		ESoulType mSoulType;

		float mHP;
		bool mbIsDistanceInPlayer;

		friend void RegisterMetaData();
	};

}
