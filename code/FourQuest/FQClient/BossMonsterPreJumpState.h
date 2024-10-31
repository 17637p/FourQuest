#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterPreJumpState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterPreJumpState();
		~BossMonsterPreJumpState() = default;

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }


		bool existBetweenWallObject(game_module::Scene* scene, const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& target, game_module::ETag tag);
		void selectJumpPosition(game_module::Animator& animator);

	private:
		// 로직 변수
		float mElapsed;
		std::shared_ptr<fq::game_module::GameObject> mDecalEffectObject;

		// 에디터 변수
		float mWaitingTime; // 점프 대기 시간
		float mTargetCaptureTime; // 움직임 확정 시간

		friend void RegisterMetaData();
	};
}