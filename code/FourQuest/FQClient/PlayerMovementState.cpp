#include "PlayerMovementState.h"

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/GameModule.h"

fq::client::PlayerMovementState::PlayerMovementState()
	:mbCanMovePlayer(false)
	,mbOnRotation(true)
	, mWalkElapsedTime()
	, mWalkSoundTurm()
{}

fq::client::PlayerMovementState::~PlayerMovementState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerMovementState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerMovementState>(*this);
}

void fq::client::PlayerMovementState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	auto controller = animator.GetComponent<game_module::CharacterController>();

	controller->SetCanMoveCharater(mbCanMovePlayer);
	controller->SetOnRotation(mbOnRotation);

	mWalkElapsedTime = 0.f;
}

void fq::client::PlayerMovementState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
{
	if (mbCanMovePlayer)
	{
		mWalkElapsedTime += dt;

		//if (mWalkElapsedTime >= mWalkSoundTurm)
		//{
		//	mWalkElapsedTime -= mWalkSoundTurm;
		//	int rand = std::rand();

		//	switch (rand)
		//	{
		//	case 0:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_1", false , 0 });
		//		break;
		//	case 1:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_2", false , 0 });
		//		break;
		//	case 2:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_3", false , 0 });
		//		break;
		//	case 3:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_4", false , 0 });
		//		break;
		//	case 4:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_5", false , 0 });
		//		break;
		//	case 5:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_6", false , 0 });
		//		break;
		//	case 6:
		//		animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_Walking_7", false , 0 });
		//		break;
		//	default:
		//		break;
		//	}
		//}
	}
}

