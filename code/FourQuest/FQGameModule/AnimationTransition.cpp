#include "AnimationTransition.h"

#include "AnimatorController.h"

bool fq::game_module::TransitionCondition::CheckCondition(Parameter parameter)
{
	auto parameterType = parameter.type();
	auto compareType = mCompareParameter.type();

	assert(parameterType == compareType);

	if (compareType == entt::resolve<int>())
	{
		int val = parameter.cast<int>();
		int compareVal = mCompareParameter.cast<int>();

		switch (mCheckType)
		{
		case fq::game_module::TransitionCondition::CheckType::Greater:
			if (val > compareVal) return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::Less:
			if (val < compareVal) return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::Equals:
			if (val == compareVal) return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::NotEqual:
			if (val != compareVal) return true;
			break;
		}
	}
	else if (compareType == entt::resolve<float>())
	{
		float val = parameter.cast<float>();
		float compareVal = mCompareParameter.cast<float>();

		switch (mCheckType)
		{
		case fq::game_module::TransitionCondition::CheckType::Greater:
			if (val > compareVal)return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::Less:
			if (val < compareVal) return true;
			break;
		}
	}
	else if (compareType == entt::resolve<bool>())
	{
		bool val = parameter.cast<bool>();
		bool compareVal = mCompareParameter.cast<bool>();

		if (val == compareVal) return true;
	}
	else if (compareType == entt::resolve<char>())
	{
		char val = parameter.cast<char>();
		char compareVal = mCompareParameter.cast<char>();

		if (val == compareVal) return true;
	}
	else assert(nullptr);

	return false;
}

fq::game_module::TransitionCondition::TransitionCondition(CheckType type, ParameterID compareID, Parameter comparCondition)
	:mCheckType(type)
	, mCompareParameter(comparCondition)
	, mParameterID(compareID)
{}

void fq::game_module::AnimationTransition::PushBackCondition(TransitionCondition::CheckType type
	, TransitionCondition::ParameterID id
	, TransitionCondition::Parameter compareParam)
{
	mConditions.push_back({ type, id, compareParam });
}

void fq::game_module::AnimationTransition::PopBackCondition()
{
	mConditions.pop_back();
}

fq::game_module::AnimationTransition::AnimationTransition(std::string prev, std::string next)
	:mNextState(next)
	,mPrevState(prev)
	,mConditions{}
{}

fq::game_module::AnimationTransition::AnimationTransition()
	:mNextState{}
	,mPrevState{}
	,mConditions{}
{
}
