#include "AnimatorController.h"

#include <spdlog/spdlog.h>

fq::game_module::AnimatorController::AnimatorController()
{
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

	if (mStates.find(name) != mStates.end())
	{
		spdlog::warn("[AnimationController] The state name is duplicated [{}]", name);
		return;
	}

	AnimationStateNode stateNode; 
	stateNode.SetType(AnimationStateNode::Type::State);
	mStates.insert({ name,stateNode });
}

void fq::game_module::AnimatorController::AddTransition(StateName prev, StateName next)
{
	for (const auto& transition : mTransitions)
	{
		if (transition.GetNextState() == next
			&& transition.GetPrevState() == prev)
		{
			return;
		}
	}

	mTransitions.push_back({ prev,next });
}
