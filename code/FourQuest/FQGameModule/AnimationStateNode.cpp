#include "AnimationStateNode.h"
#include "AnimatorController.h"
#include "IStateBehaviour.h"
#include "LogStateBehaviour.h"
#include "EventManager.h"
#include "Event.h"
#include "GameObject.h"
#include "Scene.h"

fq::game_module::AnimationStateNode::AnimationStateNode(AnimatorController* controller)
	:mController(controller)
	, mType(Type::State)
	, mPlayBackSpeed(1.f)
	, mAnimationPath{}
	, mAnimationKey{}
	, mbIsLoof(true)
	, mDuration(0.f)
	, mStartTimePos(0.f)
	, mBehaviours{}
{
}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{}

void fq::game_module::AnimationStateNode::OnStateUpdate(float dt)
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateUpdate(*mController->GetAnimator(), *this, dt);
	}

	for (auto& effectEvent : mEvents)
	{
		if (effectEvent.Time < mAccumulationTime && !effectEvent.bIsFiredReady)
		{
			effectEvent.bIsFiredReady = true;
		}
	}

	mAccumulationTime += dt;
}

void fq::game_module::AnimationStateNode::ProcessAnimationEvent(class GameObject* gameObject, EventManager* eventManager)
{
	if (gameObject->IsDestroyed())
	{
		return;
	}

	for (auto& currentEvent : mEvents)
	{
		if (currentEvent.bIsFiredReady && !currentEvent.bIsFired)
		{
			eventManager->FireEvent<fq::event::OnCreateStateEvent>({ currentEvent.FunctionName, gameObject });
			currentEvent.bIsFired = true;
		}
	}
}

void fq::game_module::AnimationStateNode::OnStateExit()
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateExit(*mController->GetAnimator(), *this);
	}

	auto eventManager = mController->GetAnimator()->GetScene()->GetEventManager();
	auto gameObject = mController->GetAnimator()->GetGameObject();

	for (auto& currentEvent : mEvents)
	{
		if (currentEvent.bIsFired)
		{
			eventManager->FireEvent<fq::event::OnDeleteStateEvent>({ currentEvent.FunctionName, gameObject });
		}
	}
}

void fq::game_module::AnimationStateNode::OnStateEnter()
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateEnter(*mController->GetAnimator(), *this);
	}

	for (auto& effectEvent : mEvents)
	{
		effectEvent.bIsFiredReady = false;
		effectEvent.bIsFired = false;
	}

	mAccumulationTime = 0.f;
}

