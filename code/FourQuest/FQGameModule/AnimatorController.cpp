#include "AnimatorController.h"

#include <spdlog/spdlog.h>

fq::game_module::AnimatorController::AnimatorController()
	:mParmeters{}
	, mStates{}
	, mCurrentState{ "Entry" }
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
