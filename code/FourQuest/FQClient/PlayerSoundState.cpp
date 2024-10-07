#include "PlayerSoundState.h"

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	PlayerSoundState::PlayerSoundState()
		: mbIsPlayLoop(false)
		, mbIsPlayStateEnter(false)
		, mSoundTurm(0.5f)
		, mbUseRandomPlay(false)
		, mSoundNames{}

		, mElapsedTime(0.f)
		, mSoundIndex(0)
		, mbIsPlaySoundOnce(false)
	{
	}

	PlayerSoundState::~PlayerSoundState()
	{
	}

	void PlayerSoundState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		mElapsedTime = 0.f;
		mSoundIndex = 0u;
		mbIsPlaySoundOnce = false;

		if (mbIsPlayStateEnter)
		{
			playSound(animator);
		}
	}

	void PlayerSoundState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		if (!mbIsPlayLoop && mbIsPlaySoundOnce)
		{
			return;
		}

		mElapsedTime += dt;

		if (mElapsedTime >= mSoundTurm)
		{
			mElapsedTime -= mSoundTurm;
			mbIsPlaySoundOnce = true;
			playSound(animator);
		}
	}
	void PlayerSoundState::playSound(fq::game_module::Animator& animator)
	{
		if (mSoundNames.empty())
		{
			return;
		}

		if (mbUseRandomPlay)
		{
			mSoundIndex = std::rand() % mSoundNames.size();
		}
		else
		{
			mSoundIndex = (mSoundIndex + 1) % mSoundNames.size();
		}

		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ mSoundNames[mSoundIndex], false , fq::sound::EChannel::SE });
	}
	std::shared_ptr<fq::game_module::IStateBehaviour> PlayerSoundState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerSoundState>(*this);
	}
}
