#pragma once

#include "EBerserkerAttackType.h"
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class Animator;
	class Transform;
	class CharacterController;
	class GameObject;
}

namespace fq::client
{
	class Player;

	enum class EBerserkerSoundType
	{
		Left,
		Right,
		StrikeDown,
		SwingAround,
		Rush,
		RushReady,
	};

	class BerserkerArmour : public fq::game_module::Component
	{
		using Vector3 = DirectX::SimpleMath::Vector3;

	public:
		BerserkerArmour() = default;
		~BerserkerArmour() = default;

		std::shared_ptr<fq::game_module::GameObject> EmitAttackIntend(EBerserkerAttackType attackType, const Vector3& offset, const Vector3& scale, float knocBackPower, float destroyTime);
		void EmitSound(EBerserkerSoundType soundType);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnUpdate(float dt) override;
		void OnStart() override;

		void checkSkillCoolTime(float dt);
		void checkInput();

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;
		game_module::Transform* mTransform;
		client::Player* mPlayer;

		// 인스펙터 변수
		game_module::PrefabResource mBoxAttackPrefab;
		game_module::PrefabResource mCircleAttackPrefab;

		std::string mLeftAttackSound;
		std::string mRightAttackSound;
		std::string mStrikeDownAttackSound;
		std::string mSwingAroundSound;
		std::string mAttackRushSound;
		std::string mAttackRushReadySound;

		std::string mLeftAttackHitSound;
		std::string mRightAttackHitSound;
		std::string mStrikeDownAttackHitSound;
		std::string mSwingAroundHitSound;
		std::string mAttackRushHitSound;

		float mSwingAroundCoolTime;
		float mRushCoolTime;

		float mSwingAroundCoolTimeReduction;
		float mRushCoolTimeReduction;

		float mTargetPosRatio = 1.f;
		float mDirectionRatio = 1.f;

		// 로직 변수
		float mSwingAroundElapsedTime;
		float mRushElapsedTime;

		friend void RegisterMetaData();
	};
}
