#pragma once

#include "../FQGameModule/IStateBehaviour.h"
#include "../FQGameModule/InputEnum.h"
#include <Xinput.h>

namespace fq::client
{
	class VibrationState : public fq::game_module::IStateBehaviour
	{
	public:
		VibrationState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(fq::game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		unsigned int NoID = XUSER_MAX_COUNT + 1;

		bool mbUseAllController; 
		EVibrationMode mMode;
		float mDuration;
		int mIntensity;
		float mElapsedTime;
		float mVibrationTime;
		unsigned int mControllerID;

		friend void RegisterMetaData();
	};
}