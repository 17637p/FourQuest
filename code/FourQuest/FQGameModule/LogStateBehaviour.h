#pragma once
#include "IStateBehaviour.h"

namespace fq::game_module
{
	class LogStateBehaviour :
		public IStateBehaviour
	{
	public:
		LogStateBehaviour();
		~LogStateBehaviour();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(Animator& animator, AnimationStateNode& state) override;
		void OnStateUpdate(Animator& animator, AnimationStateNode& state) override;
		void OnStateExit(Animator& animator, AnimationStateNode& state) override;

		unsigned int GetEnterCount() const { return mEnterCount; }
		void SetEnterCount(unsigned int val) { mEnterCount = val; }
		unsigned int GetExitCount() const { return mExitCount; }
		void SetExitCount(unsigned int val) { mExitCount = val; }
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		unsigned int mEnterCount = 0;
		unsigned int mExitCount = 0;
	};
}