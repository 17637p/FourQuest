#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class Player;

	/// <summary>
	/// 마법 공격에 대한 처리를 합니다 
	/// </summary>
	class MagicArmour : public fq::game_module::Component
	{
	public:
		MagicArmour();
		~MagicArmour();

		/// <summary>
		/// X 버튼 마법 공격을 발사합니다
		/// </summary>
		void EmitMagicBall();

		/// <summary>
		/// A 버튼 장판 마법 공격을 발사합니다
		/// </summary>
		void EmitAOE(DirectX::SimpleMath::Vector3 attackPoint);

		/// <summary>
		/// R 스틱으로 Razer를 쏩니다
		/// </summary>
		void EmitRazer();

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		game_module::Transform* mTransform;
		Player* mPlayer;

		float mMagicBallSpeed;

		game_module::PrefabResource mMagicBall;
		game_module::PrefabResource mAttackWarningUI;
		game_module::PrefabResource mAOE;
		game_module::PrefabResource mRazer;

		friend void RegisterMetaData();
	};


}