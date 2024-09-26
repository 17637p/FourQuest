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
		if (effectEvent.Time < mAccumulationTime && !effectEvent.bIsProcessed)
		{
			effectEvent.bIsProcessed = true;
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
		if (currentEvent.bIsProcessed && !currentEvent.bIsFired)
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

	if (!mEvents.empty())
	{
		eventManager->FireEvent<fq::event::OnDeleteStateEvent>({ gameObject });
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
		effectEvent.bIsProcessed = false;
		effectEvent.bIsFired = false;
	}

	mAccumulationTime = 0.f;
}

