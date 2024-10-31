#pragma once
#include "../FQGameModule/IStateBehaviour.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class BossMonsterJumpState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterJumpState();
		~BossMonsterJumpState()= default;
		 
	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		// ����
		float mElapsed;
		DirectX::SimpleMath::Vector3 mStartPosition;
		DirectX::SimpleMath::Vector3 mEndPosition;
		std::shared_ptr<fq::game_module::GameObject> mDecalEndEffectObject;
		bool mbIsEmitAttack;
		bool mbIsEmitEffect;

		// ������
		float mMaxY;
		float mJumpDuration; // ���� ���� �ð�
		float mAttackGenateTime; // ���� ���� �ð�
		float mEffectGenateTime; // ����Ʈ ���� �ð�
		friend void RegisterMetaData();
	};
}