#include "AnimatorController.h"

#include <spdlog/spdlog.h>

#include "Animator.h"
#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::AnimatorController::AnimatorController()
	:mParmeters{}
	, mStates{}
	, mCurrentState{ "Entry" }
	, mTimePos(0.f)
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
}

fq::game_module::AnimatorController::~AnimatorController()
{

}

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

void fq::game_module::AnimatorController::AddTransition(StateName exit, StateName enter)
{
	if (exit == enter) return;

	for (const auto& transition : mTransitions)
	{
		if (transition.GetEnterState() == enter
			&& transition.GetExitState() == exit)
		{
			return;
		}
	}

	mTransitions.push_back({ exit,enter });
}

void fq::game_module::AnimatorController::DeleteTransition(StateName exit, StateName enter)
{
	for (auto iter = mTransitions.begin(); iter != mTransitions.end();)
	{
		if (iter->GetEnterState() == enter
			&& iter->GetExitState() == exit)
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
	for (auto& transition : mTransitions)
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
	for (auto& transition : mTransitions)
	{
		auto exitState = transition.GetExitState();

		// AnyState or 현재상태와 일치하면 확인
		if (exitState == mCurrentState || exitState == "AnyState")
		{
			bool passCondition = checkConditions(transition);

			// 상태를 변경합니다 
			if (passCondition)
			{
				auto eventMgr = mAnimator->GetScene()->GetEventManager();

				std::string enterState = transition.GetEnterState();
				mTimePos = 0.f;
				mCurrentState = enterState;

				// 애니메이션 상태변경 이벤트
				eventMgr->FireEvent<fq::event::ChangeAnimationState>({
					exitState,
					enterState,
					mAnimator->GetGameObject()
					});

				return;
			}
		}
	}
}


bool fq::game_module::AnimatorController::checkConditions(AnimationTransition& transition)
{
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

float fq::game_module::AnimatorController::UpdateAnimation(float dt)
{
	auto& state = mStates.find(mCurrentState)->second;

	float duration = state.GetDuration();
	float playbackSpeed = state.GetPlayBackSpeed();

	mTimePos = std::fmod(mTimePos + dt * playbackSpeed, duration);
	return mTimePos;
}

void fq::game_module::AnimatorController::DeleteState(StateName state)
{
	auto iter = mStates.find(state);

	if (iter->second.GetType() != AnimationStateNode::Type::State)
		return;

	// 스테이트 삭제
	mStates.erase(iter);

	mTransitions.erase(std::find_if(mTransitions.begin(), mTransitions.end()
		, [state](const AnimationTransition& transition)
		{
			if (transition.GetEnterState() == state
				|| transition.GetExitState() == state)
			{
				return true;
			}

			return false;
		}), mTransitions.end());

}
