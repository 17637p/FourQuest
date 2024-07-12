#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class Player;

	/// <summary>
	/// ���� ���ݿ� ���� ó���� �մϴ� 
	/// </summary>
	class MagicArmour : public fq::game_module::Component
	{
	public:
		MagicArmour();
		~MagicArmour();

		/// <summary>
		/// X ��ư ���� ������ �߻��մϴ�
		/// </summary>
		void EmitMagicBall();

		/// <summary>
		/// A ��ư ���� ���� ������ �߻��մϴ�
		/// </summary>
		void EmitAOE(DirectX::SimpleMath::Vector3 attackPoint);

		/// <summary>
		/// R ��ƽ���� Razer�� ���ϴ�
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