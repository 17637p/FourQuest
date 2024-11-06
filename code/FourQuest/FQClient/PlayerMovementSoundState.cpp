#include "PlayerMovementSoundState.h"

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	PlayerMovementSoundState::PlayerMovementSoundState()
		: mWalkElapsedTime()
		, mWalkSoundTurm(0.6f)
	{
	}

	PlayerMovementSoundState::~PlayerMovementSoundState()
	{
	}

	void PlayerMovementSoundState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		mWalkElapsedTime = 0.f;
		playSound(animator);
	}

	void PlayerMovementSoundState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		mWalkElapsedTime += dt;

		if (mWalkElapsedTime >= mWalkSoundTurm)
		{
			mWalkElapsedTime -= mWalkSoundTurm;
			playSound(animator);
		}
	}
	void PlayerMovementSoundState::playSound(fq::game_module::Animator& animator)
	{
		int random = std::rand() % 7;

		switch (random)
		{
		case 0:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_1", false , fq::sound::EChannel::SE });
			break;
		case 1:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_2", false , fq::sound::EChannel::SE });
			break;
		case 2:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_3", false , fq::sound::EChannel::SE });
			break;
		case 3:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_4", false , fq::sound::EChannel::SE });
			break;
		case 4:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_5", false , fq::sound::EChannel::SE });
			break;
		case 5:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_6", false , fq::sound::EChannel::SE });
			break;
		case 6:
			animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_7", false , fq::sound::EChannel::SE });
			break;
		default:
			break;
		}
	}
	std::shared_ptr<fq::game_module::IStateBehaviour> PlayerMovementSoundState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerMovementSoundState>(*this);
	}
}
