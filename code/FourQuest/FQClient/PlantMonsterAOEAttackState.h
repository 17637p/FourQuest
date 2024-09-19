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
		float mLookAtTime;			// ���� �ߵ������� �����ϴ� �ð�
		float mLookAtElapsedTime;	// 
		float mAttackTiming;		// ���ݹߵ� �ð�
		float mElapsedTime;			// ���� �ð�

		friend void RegisterMetaData();
	};

}