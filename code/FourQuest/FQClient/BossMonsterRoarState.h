#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterRoarState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterRoarState();
		~BossMonsterRoarState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		unsigned int mMeleeNumber;
		unsigned int mExplosionNumber;
		DirectX::SimpleMath::Vector3 mPosition;

		bool mbIsEnterAngry;
		float mChangeColorDuration;
		float mChangeColorElapsed;
		DirectX::SimpleMath::Color mStartColor;
		DirectX::SimpleMath::Color mEndColor;
		float mStartInvRimPow;
		float mEndInvRimPow;

		friend void RegisterMetaData();
	};
}