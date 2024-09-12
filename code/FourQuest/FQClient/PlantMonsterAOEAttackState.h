#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class PlantMonsterAOEAttackState : public fq::game_module::IStateBehaviour
	{
	public:
		PlantMonsterAOEAttackState();
		~PlantMonsterAOEAttackState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;


		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mLookAtTime;			// 공격 발동전까지 응시하는 시간
		float mLookAtElapsedTime;	// 
		float mAttackTiming;		// 공격발동 시간
		float mElapsedTime;			// 지연 시간

		friend void RegisterMetaData();
	};

}