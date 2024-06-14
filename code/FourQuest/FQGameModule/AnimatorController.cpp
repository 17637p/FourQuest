#define NOMINMAX
#include "AnimatorController.h"

#include <limits>
#include <spdlog/spdlog.h>

#include "Animator.h"
#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::AnimatorController::AnimatorController()
	:mParmeters{}
	, mAnimator(nullptr)
	, mStates{}
	, mCurrentState{}
	, mNextState{}
	, mTimePos(0.f)
	, mBlendTimePos(0.f)
	, mBlendWeight(0.f)
	, mBlendElapsedTime(0.f)
{
	// Entry
	AnimationStateNode entry(this);
	entry.SetType(AnimationStateNode::Type::Entry);
	entry.SetAnimationKey("Entry");
	mStates.insert({ "Entry", entry });

	// Exit 
	AnimationStateNode exit(this);
	exit.SetType(AnimationStateNode::Type::Exit);
	exit.SetAnimationKey("Exit");
	mStates.insert({ "Exit",exit });

	// AnyState
	AnimationStateNode anyState(this);
	anyState.SetType(AnimationStateNode::Type::AnyState);
	anyState.SetAnimationKey("AnyState");
	mStates.insert({ "AnyState", anyState });

	mCurrentState = mStates.find("Entry");
	mNextState = mStates.end();
	mCurrentTransition = mTransitions.end();
}

fq::game_module::AnimatorController::~AnimatorController()
{}

void fq::game_module::AnimatorController::SetParameter(ParameterID id, Parameter parameter)
{
	auto iter = mParmeters.find(id);

	assert(iter != mParmeters.end() && "ParameterID 오류");
	assert(iter->second.type() == parameter.type() && "Type이 일치하지 않습니다");
	iter->second = parameter;
}

void fq::game_module::AnimatorController::AddParameter(ParameterID id, Parameter parameter)
{
	if (mParmeters.find(id) != mParmeters.end())
	{
		spdlog::warn("[AnimationController] The parameter name is duplicated [{}]", id);
		return;
	}
	mParmeters.insert({ id,parameter });
}

fq::game_module::AnimatorController::Parameter fq::game_module::AnimatorController::GetParameter(ParameterID id) const
{
	auto iter = mParmeters.find(id);

	if (iter == mParmeters.end())
	{
		return entt::meta_any();
	}

	return iter->second;
}

void fq::game_module::AnimatorController::EraseParameter(ParameterID id)
{
	auto iter = mParmeters.find(id);

	if (iter != mParmeters.end())
	{
		mParmeters.erase(iter);
	}
}

void fq::game_module::AnimatorController::CreateStateNode()
{
	std::string name = "NewState";

	while (mStates.find(name) != mStates.end())
	{
		name += "+";
	}

	AnimationStateNode stateNode(this);
	stateNode.SetType(AnimationStateNode::Type::State);
	stateNode.SetAnimationKey(name);
	mStates.insert({ name,stateNode });
}

void fq::game_module::AnimatorController::AddTransition(AnimationTransition transition)
{
	if (transition.GetEnterState() == transition.GetExitState()
		|| transition.GetExitState() == "Exit"
		|| transition.GetEnterState() == "AnyState"
		|| transition.GetEnterState() == "Entry")
	{
		return;
	}

	mTransitions.insert({ transition.GetExitState(), transition });
}

void fq::game_module::AnimatorController::DeleteTransition(StateName exit, StateName enter)
{
	for (auto iter = mTransitions.begin(); iter != mTransitions.end();)
	{
		if (iter->second.GetEnterState() == enter
			&& iter->second.GetExitState() == exit)
		{
			iter = mTransitions.erase(iter);
		}
		else ++iter;
	}
}

bool fq::game_module::AnimatorController::ChangeStateName(StateName orginName, StateName changeName)
{
	auto iter = mStates.find(orginName);

	if (iter == mStates.end()) return false;

	AnimationStateNode state = iter->second;

	if (state.GetType() != AnimationStateNode::Type::State)
		return false;

	state.SetAnimationKey(changeName);
	mStates.erase(iter);
	mStates.insert({ changeName, state });

	// Transition 변경
	for (auto& [stateName, transition] : mTransitions)
	{
		if (transition.GetEnterState() == orginName)
		{
			transition.SetEnterState(changeName);
		}

		if (transition.GetExitState() == orginName)
		{
			transition.SetExitState(changeName);
		}
	}

	return true;
}

void fq::game_module::AnimatorController::AddStateNode(AnimationStateNode node)
{
	assert(mStates.find(node.GetAnimationKey()) == mStates.end());
	mStates.insert({ node.GetAnimationKey(), node });
}

void fq::game_module::AnimatorController::UpdateState(float dt)
{
	if (!IsInTransition())
	{
		auto transition = checkStateTransition(mCurrentState->first);

		// 애니메이션 전환발생
		if (transition != mTransitions.end())
		{
			// 바로 전환
			if (transition->second.GetTransitionDuration() <= std::numeric_limits<float>::epsilon())
			{
				mCurrentState->second.OnStateExit();
				mCurrentState = mStates.find(transition->second.GetEnterState());
				mCurrentState->second.OnStateEnter();
				mCurrentTransition = mTransitions.end();
				mTimePos = 0.f;
				emitChangeState();
			}
			else
			{
				mNextState = mStates.find(transition->second.GetEnterState());
				mNextState->second.OnStateEnter();
				mCurrentTransition = transition;
				emitChangeState();
			}
		}
	}

	// 애니메이션 전환 종료
	else if (EndTransitionWeight == mBlendWeight)
	{
		auto eventMgr = mAnimator->GetScene()->GetEventManager();

		mCurrentState->second.OnStateExit();
		mCurrentState = mNextState;
		mNextState = mStates.end();
		mCurrentTransition = mTransitions.end();
		mTimePos = mBlendTimePos;
		mBlendTimePos = 0.f;
		mBlendWeight = 0.f;
		mBlendElapsedTime = 0.f;

		emitChangeState();
	}
	else // 3. 애니메이션 전환중인 경우
	{
		using  Source = AnimationTransition::InterruptionSource;

		// 애니메이션 전환중 다른 전환 처리 
		auto source = mCurrentTransition->second.GetInterruptionSource();

		if (source == Source::NextState)
			checkNextStateTransition();
		else if (source == Source::CurrentState)
			checkCurrentStateTransition();
		else if (source == Source::CurrentStateThenNextState)
		{
			bool pass = checkCurrentStateTransition();

			if (!pass)
				checkNextStateTransition();
		}
		else if (source == Source::NextStateThenCurrentState)
		{
			bool pass = checkNextStateTransition();

			if (!pass)
				checkCurrentStateTransition();
		}
	}
}


bool fq::game_module::AnimatorController::checkConditions(const AnimationTransition& transition, float timePos)
{
	// ExitTime
	if (transition.HasExitTime()
		&& transition.GetExitTime() > timePos)
	{
		return false;
	}

	// 조건이 없으면 true 
	if (transition.GetConditions().empty())
		return true;

	// 전이 조건을 체크합니다 
	for (const auto& condition : transition.GetConditions())
	{
		auto iter = mParmeters.find(condition.GetParameterID());
		assert(iter != mParmeters.end());
		if (!condition.CheckCondition(iter->second))
		{
			return false;
		}
	}

	// 트리거 조건인 경우에 off상태로 바꾼다
	for (const auto& condition : transition.GetConditions())
	{
		auto iter = mParmeters.find(condition.GetParameterID());

		if (iter->second.type() == entt::resolve<char>())
		{
			iter->second = OffTrigger;
		}
	}

	return true;
}

void fq::game_module::AnimatorController::UpdateAnimation(float dt)
{
	if (IsInTransition())
	{
		// current 
		float currentDuration = mCurrentState->second.GetDuration();
		float currentPlayBackSpeed = mCurrentState->second.GetPlayBackSpeed();
		bool currentLoof = mCurrentState->second.IsLoof();

		mTimePos = std::min(mTimePos + dt * currentPlayBackSpeed, currentDuration);

		// next
		float nextDuration = mNextState->second.GetDuration();
		float nextPlayBackSpeed = mNextState->second.GetPlayBackSpeed();
		bool nextLoof = mNextState->second.IsLoof();

		mBlendTimePos = std::min(mBlendTimePos + dt * nextPlayBackSpeed, nextDuration);

		float transitionDuration = mCurrentTransition->second.GetTransitionDuration();
		mBlendElapsedTime = std::min(mBlendElapsedTime + dt, transitionDuration);
		mBlendWeight = mBlendElapsedTime / transitionDuration;

		if (mBlendElapsedTime == transitionDuration)
			mBlendWeight = EndTransitionWeight;
	}
	else
	{
		auto& state = mCurrentState->second;
		float duration = state.GetDuration();
		float playbackSpeed = state.GetPlayBackSpeed();
		bool isLoof = state.IsLoof();

		if (isLoof)
			mTimePos = std::fmod(mTimePos + dt * playbackSpeed, duration);
		else
			mTimePos = std::min(mTimePos + dt * playbackSpeed, duration);
	}
}

void fq::game_module::AnimatorController::DeleteState(StateName state)
{
	auto iter = mStates.find(state);

	if (iter->second.GetType() != AnimationStateNode::Type::State)
		return;

	// 스테이트 삭제
	mStates.erase(iter);
	mTransitions.erase(state);

	for (auto iter = mTransitions.begin(); iter != mTransitions.end();)
	{
		if (iter->second.GetEnterState() == state)
		{
			iter = mTransitions.erase(iter);
		}
		else
			++iter;
	}

}

bool fq::game_module::AnimatorController::IsInTransition() const
{
	return mNextState != mStates.end();
}

float fq::game_module::AnimatorController::GetBlendWeight() const
{
	return mBlendWeight;
}

fq::game_module::AnimatorController::TransitionIterator fq::game_module::AnimatorController::GetCurrentTransition()
{
	return IsInTransition() ? mCurrentTransition : mTransitions.end();
}

fq::game_module::AnimatorController::StateName fq::game_module::AnimatorController::GetNextStateName() const
{
	if (mNextState == mStates.end())
	{
		return {};
	}

	return mNextState->first;
}

fq::game_module::AnimatorController::TransitionIterator fq::game_module::AnimatorController::checkStateTransition(const StateName& name)
{
	// State 조건
	for (auto [beginIter, endIter]
		= mTransitions.equal_range(name);
		beginIter != endIter; ++beginIter)
	{
		bool passCondition = checkConditions(beginIter->second, mTimePos);

		if (passCondition)
			return beginIter;
	}

	// AnyState 조건
	for (auto [beginIter, endIter]
		= mTransitions.equal_range("AnyState");
		beginIter != endIter; ++beginIter)
	{
		bool passCondition = checkConditions(beginIter->second, mTimePos);

		if (passCondition)
			return beginIter;
	}

	return mTransitions.end();
}

void fq::game_module::AnimatorController::emitChangeState()
{
	auto eventMgr = mAnimator->GetScene()->GetEventManager();

	bool isBlend = (mNextState != mStates.end());

	if (isBlend)
	{
		// Entry는 바로 전환합니다 
		if (mCurrentState->first == "Entry")
		{
			mCurrentState = mNextState;
			mNextState = mStates.end();
			isBlend = false;

			eventMgr->FireEvent<fq::event::ChangeAnimationState>({
				isBlend,
				mCurrentState->first,
				"",
				mAnimator
				});
		}
		else
		{
			eventMgr->FireEvent<fq::event::ChangeAnimationState>({
				isBlend,
				mCurrentState->first,
				mNextState->first,
				mAnimator
				});
		}
	}
	else
	{
		eventMgr->FireEvent<fq::event::ChangeAnimationState>({
			isBlend,
			mCurrentState->first,
			{},
			mAnimator
			});
	}
}

bool fq::game_module::AnimatorController::checkNextStateTransition()
{
	auto transition = checkStateTransition(mNextState->first);

	// Interrupt 발생 
	if (transition != mTransitions.end())
	{
		// 애니메이션 블렌딩없이 전환
		if (transition->second.GetTransitionDuration() <= std::numeric_limits<float>::epsilon())
		{
			mCurrentState->second.OnStateExit();
			mNextState->second.OnStateExit();
			mNextState = mStates.end();
			mCurrentState = mStates.find(transition->second.GetEnterState());
			mCurrentState->second.OnStateEnter();

			mTimePos = 0.f;
			mBlendTimePos = 0.f;
			mBlendWeight = 0.f;
			mBlendElapsedTime = 0.f;
			mCurrentTransition = mTransitions.end();
		}
		else
		{
			mCurrentState->second.OnStateExit();
			mCurrentState = mNextState;
			mNextState = mStates.find(transition->second.GetEnterState());
			mNextState->second.OnStateEnter();

			mTimePos = mBlendTimePos;
			mBlendTimePos = 0.f;
			mBlendWeight = 0.f;
			mBlendElapsedTime = 0.f;
			mCurrentTransition = transition;
		}
		emitChangeState();
		return true;
	}

	return false;
}

bool fq::game_module::AnimatorController::checkCurrentStateTransition()
{
	auto transition = checkStateTransition(mCurrentState->first);

	// Interrupt 발생 
	if (transition != mTransitions.end())
	{
		// 애니메이션 블렌딩없이 전환
		if (transition->second.GetTransitionDuration() <= std::numeric_limits<float>::epsilon())
		{
			mNextState->second.OnStateExit();
			mNextState = mStates.end();
			mCurrentState->second.OnStateExit();
			mCurrentState = mStates.find(transition->second.GetEnterState());

			mTimePos = 0.f;
			mBlendTimePos = 0.f;
			mBlendWeight = 0.f;
			mBlendElapsedTime = 0.f;
			mCurrentTransition = mTransitions.end();
		}
		else
		{
			mNextState->second.OnStateExit();
			mNextState = mStates.find(transition->second.GetEnterState());
			mNextState->second.OnStateEnter();

			mBlendTimePos = 0.f;
			mBlendWeight = 0.f;
			mBlendElapsedTime = 0.f;
			mCurrentTransition = transition;
		}
		emitChangeState();
		return true;
	}

	return false;
}

void fq::game_module::AnimatorController::Update(float dt)
{
	mCurrentState->second.OnStateUpdate(dt);

	if (mNextState != mStates.end())
	{
		mNextState->second.OnStateUpdate(dt);
	}
}
