#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class PlayerSoundState : public fq::game_module::IStateBehaviour
	{
	public:
		PlayerSoundState();
		~PlayerSoundState();

		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt) override;

	private:
		void playSound(fq::game_module::Animator& animator);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<IStateBehaviour> Clone() override;

	private:
		bool mbIsPlayLoop;
		bool mbIsPlayStateEnter;
		float mSoundTurm;
		bool mbUseRandomPlay;
		std::vector<std::string> mSoundNames;

		float mElapsedTime;
		unsigned int mSoundIndex;
		bool mbIsPlaySoundOnce;

		friend void RegisterMetaData();
	};
}