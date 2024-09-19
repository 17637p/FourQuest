#pragma once
#include "../FQGameModule/IStateBehaviour.h"

#include <directxtk/SimpleMath.h>

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	class AOEAttackState :public fq::game_module::IStateBehaviour
	{
	public:
		AOEAttackState();
		~AOEAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		fq::game_module::GameObject* mAttackWarningUI;

		DirectX::SimpleMath::Vector3 mCenterPosition;
		float mMoveRange;

		unsigned int mControllerID;
	};


}