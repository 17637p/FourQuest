#define NOMINMAX
#include "AnimatorController.h"

#include <limits>
#include <spdlog/spdlog.h>
#include <cassert>

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

bool fq::game_module::AnimatorController::SetParameter(ParameterID id, Parameter parameter)
{
	auto iter = mParmeters.find(id);

	if (iter == mParmeters.end())
	{
		return false;
	}

	assert(iter->second.type() == parameter.type() && "Type이 일치하지 않습니다");
	iter->second = parameter;

	return true;
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
			// 즉시 전환
			if (transition->second.GetTransitionDuration() <= std::numeric_limits<float>::epsilon())
			{
				mNextState = mStates.find(transition->second.GetEnterState());
				mCurrentState->second.OnStateExit();
				mNextState->second.OnStateEnter();
				mCurrentState = mNextState;
				mNextState = mStates.end();
				mCurrentTransition = mTransitions.end();
				mTimePos = mCurrentState->second.GetStartTimePos();
				emitChangeState();
			}
			else
			{
				mNextState = mStates.find(transition->second.GetEnterState());
				mNextState->second.OnStateEnter();
				mCurrentTransition = transition;
				mBlendTimePos = mNextState->second.GetStartTimePos();
				mBlendWeight = 0.f;
				mBlendElapsedTime = 0.f;

				emitChangeState();
			}
		}
	}

	// 애니메이션 전환 종료
	else if (EndTransitionWeight == mBlendWeight)
	{
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

void fq::game_module::AnimatorController::UpdateAnimation(float dt, float defaultSpeed)
{
	if (IsInTransition())
	{
		// current 
		float currentDuration = mCurrentState->second.GetDuration();
		float currentPlayBackSpeed = mCurrentState->second.GetPlayBackSpeed();
		bool currentLoof = mCurrentState->second.IsLoof();

		mTimePos = std::min(mTimePos + dt * currentPlayBackSpeed * defaultSpeed, currentDuration);

		// next
		float nextDuration = mNextState->second.GetDuration();
		float nextPlayBackSpeed = mNextState->second.GetPlayBackSpeed();
		bool nextLoof = mNextState->second.IsLoof();

		mBlendTimePos = std::min(mBlendTimePos + dt * nextPlayBackSpeed * defaultSpeed, nextDuration);

		float transitionDuration = mCurrentTransition->second.GetTransitionDuration();

		mBlendElapsedTime = std::min(mBlendElapsedTime + dt * defaultSpeed, transitionDuration);
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
			mTimePos = std::fmod(mTimePos + dt * playbackSpeed * defaultSpeed, duration);
		else
			mTimePos = std::min(mTimePos + dt * playbackSpeed * defaultSpeed, duration);
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

void fq::game_module::AnimatorController::ProcessAnimationEvent(class GameObject* gameObject, EventManager* eventManager)
{
	if (mNextState != mStates.end())
		mNextState->second.ProcessAnimationEvent(gameObject, eventManager);
	else
		mCurrentState->second.ProcessAnimationEvent(gameObject, eventManager);
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

fq::graphics::IAnimation& fq::game_module::AnimatorController::GetNextStateAnimation() const
{
	assert(mNextState != mStates.end());
	return mNextState->second.GetAnimation();
}

std::shared_ptr<fq::graphics::IAnimation> fq::game_module::AnimatorController::GetSharedNextStateAnimationOrNull() const
{
	if (mNextState == mStates.end())
	{
		return nullptr;
	}

	return mNextState->second.GetSharedAnimation();
}

const std::shared_ptr<fq::graphics::IAnimation>& fq::game_module::AnimatorController::GetSharedRefNextStateAnimation() const
{
	assert(mNextState != mStates.end());
	return mNextState->second.GetSharedRefAnimation();
}

bool fq::game_module::AnimatorController::GetHasNextStateAnimation() const
{
	return mNextState != mStates.end() && mNextState->second.GetHasAnimation();
}

fq::game_module::AnimatorController::TransitionIterator fq::game_module::AnimatorController::checkStateTransition(const StateName& name)
{
	std::vector<TransitionIterator> transitions;

	// State 조건
	for (auto [beginIter, endIter]
		= mTransitions.equal_range(name);
		beginIter != endIter; ++beginIter)
	{
		transitions.push_back(beginIter);
	}

	// AnyState 조건
	for (auto [beginIter, endIter]
		= mTransitions.equal_range("AnyState");
		beginIter != endIter; ++beginIter)
	{
		// EnterState가 같은 상태인지 확인합니다
		if (!beginIter->second.CanTrasitionToSelf()
			&& beginIter->second.GetEnterState() == name)
		{
			continue;
		}

		transitions.push_back(beginIter);
	}

	// ExitTime이 작은 순서대로 정렬
	std::sort(transitions.begin(), transitions.end(), [](const TransitionIterator& lhs, const TransitionIterator& rhs)
		{
			if (lhs->second.GetExitTime() < rhs->second.GetExitTime())
			{
				return true;
			}
			return false;
		});


	// 전환조건 확인
	for (auto& iter : transitions)
	{
		bool passCondition = checkConditions(iter->second, mTimePos);

		if (passCondition)
			return iter;
	}

	return mTransitions.end();
}

void fq::game_module::AnimatorController::emitChangeState()
{
	bool isBlend = (mNextState != mStates.end());

	if (isBlend && mCurrentState->first == "Entry")
	{
		mCurrentState = mNextState;
		mNextState = mStates.end();
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

			mTimePos = mCurrentState->second.GetStartTimePos();
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
			mBlendTimePos = mNextState->second.GetStartTimePos();
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

			mTimePos = mCurrentState->second.GetStartTimePos();
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

			mBlendTimePos = mNextState->second.GetStartTimePos();
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
	///  애니메이션 전환중에는 NextState에 대한 Update 처리를 진행합니다

	if (mNextState != mStates.end())
		mNextState->second.OnStateUpdate(dt);
	else
		mCurrentState->second.OnStateUpdate(dt);
}

